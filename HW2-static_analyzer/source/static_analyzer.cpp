#include "static_analyzer.h"
#include <regex>
#include <sstream>

void static_analyzer::input_lib(std::istream& is) {
    std::stringstream buffer;
    buffer << is.rdbuf();
    std::string table_input = buffer.str();
    buffer.clear();
    buffer.str("");
    pre_processing_lib(table_input);
    std::smatch match;
    while (std::regex_search(table_input, match, std::regex(R"(cell\s*\(\s*(\w+)\s*\)\s*\{)"))) {
        std::string cell_name = match[1];
        for (int i = 0; i < BAD_CELL; ++i) {
            if (cell_name.find(CELL_TYPE_NAME[i]) != std::string::npos) {
                this->name_to_cell_type[cell_name] = i;
                break;
            }
        }
        table_input = match.suffix();
        std::string cell_content;
        find_closing_pair(table_input, cell_content);
        while (std::regex_search(cell_content, match, std::regex(R"(pin\s*\(\s*(\w+)\s*\)\s*\{)"))) {
            std::string pin_name = match[1];
            cell_content = match.suffix();
            std::string pin_content;
            find_closing_pair(cell_content, pin_content);
            bool is_output;
            while (std::regex_search(pin_content, match, std::regex(R"(direction\s*:\s*(\w+)\s*;)"))) {
                is_output = match[1] == "output";
                pin_content = match.prefix().str() + match.suffix().str();
            }
            while (std::regex_search(pin_content, match, std::regex(R"(capacitance\s*:\s*([\d\.]+)\s*;)"))) {
                this->table_list[0]->add_port_information(cell_name, pin_name, std::make_pair(is_output, std::stod(match[1])));
                for (auto t: this->table_list) {
                    t->resize_table();
                }
                pin_content = match.prefix().str() + match.suffix().str();
            }
            if (is_output) {
                while (std::regex_search(pin_content, match, std::regex(R"(\w+\s*\(\s*\)\s*\{)"))) {
                    pin_content = match.suffix();
                    std::string internal_pin_content;
                    find_closing_pair(pin_content, internal_pin_content);
                    while (std::regex_search(internal_pin_content, match, std::regex(R"((\w+)\s*\(\s*(\w+)\s*\)\s*\{)"))) {
                        std::string type_name = match[1];
                        std::string table_name = match[2];
                        internal_pin_content = match.suffix();
                        std::string values_content;
                        find_closing_pair(internal_pin_content, values_content);
                        std::vector<std::vector<double>> table_values;
                        std::vector<double> temp_v;
                        table* t = this->name_to_table[table_name];
                        std::regex value_pattern(R"([\d\.]+)");
                        auto value_iter = std::sregex_iterator(values_content.begin(), values_content.end(), value_pattern);
                        std::sregex_iterator value_end;
                        for (std::sregex_iterator i = value_iter; i != value_end; ++i) {
                            temp_v.push_back(std::stod(i->str()));
                            if (temp_v.size() == t->get_total_output_net_capacitance_size()) {
                                table_values.push_back(temp_v);
                                temp_v.clear();
                            }
                            if (table_values.size() == t->get_input_transition_time_size()) {
                                break;
                            }
                        }
                        t->set_table(cell_name, type_name, table_values);
                    }
                }
            }
        }
    }
}

void static_analyzer::input_verilog(std::istream& is) {
    std::stringstream buffer;
    buffer << is.rdbuf();
    std::string netlist = buffer.str();
    buffer.clear();
    buffer.str("");
    pre_processing_verilog(netlist);
    std::smatch match;
    while (std::regex_search(netlist, match, std::regex(R"((\w+)\s+(\w+)\s*\()"))) {
        std::string cell_type = match[1];
        std::string cell_name = match[2];
        cell* c = new cell();
        c->set_model_name(cell_type);
        c->set_name(cell_name);
        c->set_type(this->name_to_cell_type[cell_type]);
        netlist = match.suffix();
        std::string cell_content;
        find_closing_pair(netlist, cell_content);
        while (std::regex_search(cell_content, match, std::regex(R"(\.(\w+)\s*\(\s*(\w+)\s*\))"))) {
            std::string port = match[1];
            std::string wire_name = match[2];
            cell_content = match.suffix();
            wire* w = this->name_to_wire[wire_name];
            bool is_output = this->table_list[0]->get_port_is_output(cell_type, port);
            if (is_output) {
                c->set_output_port_wire(port, w);
                w->set_input_cell_port(c, port);
            }
            else {
                c->add_input_port_wire(port, w);
                w->add_output_cell_port(c, port);
            }
        }
        this->cell_list.push_back(c);
    }
}

void static_analyzer::input_pattern(std::istream& is) {
    std::stringstream buffer;
    buffer << is.rdbuf();
    std::string pattern_str = buffer.str();
    buffer.clear();
    buffer.str("");
    std::smatch match;
    std::sregex_iterator pins_end;
    while (std::regex_search(pattern_str, match, std::regex(R"(input\s+([^\n]+))"))) {
        this->PI_list.clear();
        std::string inputs = match[1];
        std::regex pin_regex(R"(\b(\w+)\b)");
        auto pins_begin = std::sregex_iterator(inputs.begin(), inputs.end(), pin_regex);
        for (std::sregex_iterator i = pins_begin; i != pins_end; ++i) {
            wire* w = this->name_to_wire[(*i).str()];
            this->PI_list.push_back(w);
        }
        pattern_str = match.suffix();
    }
    pattern_str = std::regex_replace(pattern_str, std::regex(R"(.end[^]*)"), "");
    std::vector<int> pattern_line;
    std::regex bit_regex(R"([01])");
    auto bit_begin = std::sregex_iterator(pattern_str.begin(), pattern_str.end(), bit_regex);
    for (std::sregex_iterator i = bit_begin; i != pins_end; ++i) {
        pattern_line.push_back(stoi((*i).str()));
        if (pattern_line.size() == this->PI_list.size()) {
            this->pattern.push_back(pattern_line);
            pattern_line.clear();
        }
    }
}

void static_analyzer::calculate_and_output_capacitance(std::ostream& os) {
    for (const auto w: this->PO_list) {
        for (auto c: w->get_input_cell_port()) {
            c.first->set_output_capacitance(this->PO_capacitance);
        }
    }
    for (const auto w: this->wire_list) {
        double capacitance;
        for (auto c: w->get_input_cell_port()) {
            capacitance = c.first->get_output_capacitance();
            capacitance += this->table_list[0]->get_port_capacitance(c.first->get_model_name(), c.second);
        }
        for (auto c: w->get_output_cell_port()) {
            capacitance += this->table_list[0]->get_port_capacitance(c.first->get_model_name(), c.second);
        }
        for (auto c: w->get_input_cell_port()) {
            c.first->set_output_capacitance(capacitance);
            c.first->set_switching_power(capacitance * this->operation_voltage * this->operation_voltage * 0.5);
        }
    }
    std::string output_str = "";
    for (const auto c: this->cell_list) {
        output_str += c->get_name() + " " + std::to_string(c->get_output_capacitance()) + "\n";
    }
    os << output_str;
}

void static_analyzer::calculate_and_output_delay_power_toggle(std::ostream& os_delay, std::ostream& os_power, std::ostream& os_toggle) {
    std::string output_str_delay, output_str_power, output_str_toggle;
    std::vector<wire*> root;
    std::copy_if(
        this->wire_list.begin(),
        this->wire_list.end(),
        std::back_inserter(root),
        [](const wire* w) {
            return w->get_output_cell_port_size() == 0;
        }
    );
    for (auto t: this->table_list) {
        for (auto c: this->cell_list) {
            c->reset_toggle_time_0_1();
            c->reset_toggle_time_1_0();
            c->set_transition(FALL);
        }
        this->table_now = t;
        output_str_delay += t->get_name() + "\n";
        output_str_power += t->get_name() + "\n";
        output_str_toggle += t->get_name() + "\n";
        for (unsigned int i = 0; i < this->pattern.size(); ++i) {
            this->total_power = 0;
            unsigned int toggle_time = 0;
            for (unsigned int j = 0; j < this->PI_list.size(); ++j) {
                this->PI_list[j]->set_value(this->pattern[i][j]);
            }
            for (auto c: this->cell_list) {
                c->reset_flag();
            }
            for (const auto o: root) {
                recursive_wire(o);
            }
            for (const auto c: this->cell_list) {
                output_str_delay += c->get_name() + " " + std::to_string(c->get_transition()) + " " + std::to_string(c->get_delay()) + " " + std::to_string(c->get_output_transition()) + "\n";
                output_str_power += c->get_name() + " " + std::to_string(c->get_internal_power()) + " " + std::to_string(c->get_switching_power()) + "\n";
                toggle_time += (c->get_toggle_time_0_1() > this->max_toggle_time) ? this->max_toggle_time : c->get_toggle_time_0_1();
                toggle_time += (c->get_toggle_time_1_0() > this->max_toggle_time) ? this->max_toggle_time : c->get_toggle_time_1_0();
            }
            output_str_delay += "\n";
            output_str_power += "\n";
            output_str_toggle += std::to_string(i + 1) + " " + std::to_string(this->total_power) + " " + std::to_string(50.0 * toggle_time / this->cell_list.size() / this->max_toggle_time) + "%\n";
            output_str_toggle += "\n";
        }
    }
    os_delay << output_str_delay;
    os_power << output_str_power;
    os_toggle << output_str_toggle;
}

static_analyzer::static_analyzer() {
    this->name = "";
    this->time_unit = {1, "ns"};
    this->voltage_unit = {1, "V"};
    this->current_unit = {1, "mA"};
    this->capacitive_load_unit = {1, "pF"};
    this->table_now = nullptr;
    this->PO_capacitance = 0.03;
    this->wire_delay = 0.005;
    this->operation_voltage = 0.9;
    this->total_power = 0;
    this->max_toggle_time = 20;
}

static_analyzer::~static_analyzer() {
    for (auto t: this->table_list) {
        delete t;
    }
    this->table_list.clear();
    this->table_now = nullptr;
    for (auto c: this->cell_list) {
        delete c;
    }
    this->cell_list.clear();
    this->PI_list.clear();
    this->PO_list.clear();
    for (auto w: this->wire_list) {
        delete w;
    }
    this->wire_list.clear();
    this->name_to_wire.clear();
    this->name_to_table.clear();
    this->name_to_cell_type.clear();
    for (auto p: this->pattern) {
        p.clear();
    }
    this->pattern.clear();
}

void static_analyzer::pre_processing_lib(std::string& str) {
    str = std::regex_replace(str, std::regex("//.*|/\\*(.|\\n)*?\\*/"), "");
    std::unordered_map<std::string, std::pair<std::string, std::string>> units;
    std::unordered_map<std::string, std::pair<double, std::string>*> unit_map = {
        {"time_unit", &this->time_unit},
        {"voltage_unit", &this->voltage_unit},
        {"current_unit", &this->current_unit},
        {"capacitive_load_unit", &this->capacitive_load_unit}
    };
    std::smatch match;
    while (std::regex_search(str, match, std::regex(R"((\w+_unit)\s*:\s*\"\s*(\d+\.?\d*)\s*([a-zA-Z]+)\s*\"\s*;)"))) {
        units[match[1]] = {match[2], match[3]};
        str = match.prefix().str() + match.suffix().str();
    }
    while (std::regex_search(str, match, std::regex(R"((\w+_unit)\s*\(\s*([^,]+)\s*,\s*(\w+)\s*\)\s*;)"))) {
        units[match[1]] = {match[2], match[3]};
        str = match.prefix().str() + match.suffix().str();
    }
    for (auto u: units) {
        *(unit_map[u.first]) = {std::stod(u.second.first), u.second.second};
    }
    std::unordered_map<std::string, double> capacitive_load_map = {
        {"yF", 1e-12},
        {"zF", 1e-9},
        {"aF", 1e-6},
        {"fF", 1e-3},
        {"pF", 1},
        {"nF", 1e3},
        {"uF", 1e6},
        {"mF", 1e9},
        {"cF", 1e10},
        {"dF", 1e11},
        {"F", 1e12},
        {"daF", 1e13},
        {"hF", 1e14},
        {"kF", 1e15},
        {"MF", 1e18},
        {"GF", 1e21},
        {"TF", 1e24},
        {"PF", 1e27},
        {"EF", 1e30},
        {"ZF", 1e33},
        {"YF", 1e36}
    };
    std::unordered_map<std::string, double> time_map = {
        {"ys", 1e-15},
        {"zs", 1e-12},
        {"as", 1e-9},
        {"fs", 1e-6},
        {"ps", 1e-3},
        {"ns", 1},
        {"us", 1e3},
        {"ms", 1e6},
        {"cs", 1e7},
        {"ds", 1e8},
        {"s", 1e9},
        {"das", 1e10},
        {"hs", 1e11},
        {"ks", 1e12},
        {"Ms", 1e15},
        {"Gs", 1e18},
        {"Ts", 1e21},
        {"Ps", 1e24},
        {"Es", 1e27},
        {"Zs", 1e30},
        {"Ys", 1e33}
    };
    std::unordered_map<std::string, double> voltage_map = {
        {"yV", 1e-24},
        {"zV", 1e-21},
        {"aV", 1e-18},
        {"fV", 1e-15},
        {"pV", 1e-12},
        {"nV", 1e-9},
        {"uV", 1e-6},
        {"mV", 1e-3},
        {"cV", 1e-2},
        {"dV", 1e-1},
        {"V", 1},
        {"daV", 1e1},
        {"hV", 1e2},
        {"kV", 1e3},
        {"MV", 1e6},
        {"GV", 1e9},
        {"TV", 1e12},
        {"PV", 1e15},
        {"EV", 1e18},
        {"ZV", 1e21},
        {"YV", 1e24}
    };
    this->PO_capacitance /= this->capacitive_load_unit.first * capacitive_load_map[this->capacitive_load_unit.second];
    this->wire_delay /= this->time_unit.first * time_map[this->time_unit.second];
    this->operation_voltage /= this->voltage_unit.first * voltage_map[this->voltage_unit.second];
    while (std::regex_search(str, match, std::regex(R"(lu_table_template\s*\(\s*(\w+)\s*\)\s*\{([^}]*)\})"))) {
        table* t = new table();
        std::string table_content = match[1];
        t->set_name(table_content);
        table_content = match[2];
        std::unordered_map<std::string, std::string> variables;
        std::unordered_map<std::string, std::string> indices;
        std::smatch table_match;
        while (std::regex_search(table_content, table_match, std::regex(R"(variable_(\d+)\s*:\s*(\w+)\s*;)"))) {
            variables[table_match[1]] = table_match[2];
            table_content = table_match.prefix().str() + table_match.suffix().str();
        }
        while (std::regex_search(table_content, table_match, std::regex(R"(index_(\d+)\s*\(\s*\"([^\"]+)\"\s*\)\s*;)"))) {
            indices[table_match[1]] = table_match[2];
            table_content = table_match.prefix().str() + table_match.suffix().str();
        }
        for (auto var: variables) {
            std::vector<double> index_values;
            std::string index_str = indices[var.first];
            std::regex value_pattern(R"([\d\.]+)");
            std::sregex_iterator iter(index_str.begin(), index_str.end(), value_pattern);
            std::sregex_iterator end;
            while (iter != end) {
                index_values.push_back(std::stod(iter->str()));
                ++iter;
            }
            t->set_index(var.second, index_values);
        }
        this->table_list.push_back(t);
        this->name_to_table[t->get_name()] = t;
        str = match.suffix();
    }
}

void static_analyzer::pre_processing_verilog(std::string& str) {
    str = std::regex_replace(str, std::regex("//.*|/\\*(.|\\n)*?\\*/"), "");
    std::regex pin_regex(R"(\b(\w+)\b)");
    std::smatch match;
    std::sregex_iterator pins_end;
    while (std::regex_search(str, match, std::regex(R"(module\s+(\w+)\s*\(([^)]*)\)\s*;)"))) {
        this->name = match[1];
        std::string inputs = match[2];
        auto pins_begin = std::sregex_iterator(inputs.begin(), inputs.end(), pin_regex);
        for (std::sregex_iterator i = pins_begin; i != pins_end; ++i) {
            wire* w = new wire();
            w->set_name((*i).str());
            this->name_to_wire[w->get_name()] = w;
        }
        str = match.suffix();
    }
    while (std::regex_search(str, match, std::regex(R"(input\s+([^;]+);)"))) {
        std::string inputs = match[1];
        auto pins_begin = std::sregex_iterator(inputs.begin(), inputs.end(), pin_regex);
        for (std::sregex_iterator i = pins_begin; i != pins_end; ++i) {
            wire* w = this->name_to_wire[(*i).str()];
            this->PI_list.push_back(w);
            this->wire_list.push_back(w);
        }
        str = match.prefix().str() + match.suffix().str();
    }
    while (std::regex_search(str, match, std::regex(R"(output\s+([^;]+);)"))) {
        std::string inputs = match[1];
        auto pins_begin = std::sregex_iterator(inputs.begin(), inputs.end(), pin_regex);
        for (std::sregex_iterator i = pins_begin; i != pins_end; ++i) {
            wire* w = this->name_to_wire[(*i).str()];
            this->PO_list.push_back(w);
            this->wire_list.push_back(w);
        }
        str = match.prefix().str() + match.suffix().str();
    }
    while (std::regex_search(str, match, std::regex(R"(wire\s+([^;]+);)"))) {
        std::string inputs = match[1];
        auto pins_begin = std::sregex_iterator(inputs.begin(), inputs.end(), pin_regex);
        for (std::sregex_iterator i = pins_begin; i != pins_end; ++i) {
            wire* w = new wire();
            w->set_name((*i).str());
            this->wire_list.push_back(w);
            this->name_to_wire[w->get_name()] = w;
        }
        str = match.prefix().str() + match.suffix().str();
    }
    str = std::regex_replace(str, std::regex(R"(endmodule[^]*)"), "");
}

void static_analyzer::recursive_wire(const wire* w) {
    for (auto c: w->get_input_cell_port()) {
        if (!c.first->get_flag()) {
            c.first->set_flag();
            bool flag = false;
            double total_delay = 0;
            double input_transition = 0;
            int value;
            for (auto i: c.first->get_input_port_wire()) {
                recursive_wire(i.second);
                if (flag && i.second->get_value() == CV[c.first->get_type()]) {
                    if (i.second->get_input_cell_port_size() == 0) {
                        total_delay = 0;
                        input_transition = 0;
                    }
                    for (auto j: i.second->get_input_cell_port()) {
                        if (j.first->get_total_delay() + this->wire_delay < total_delay) {
                            total_delay = j.first->get_total_delay() + this->wire_delay;
                            input_transition = j.first->get_output_transition();
                        }
                        else if (j.first->get_total_delay() + this->wire_delay == total_delay) {
                            if (j.first->get_output_transition() < input_transition) {
                                input_transition = j.first->get_output_transition();
                            }
                        }
                    }
                }
                else if (i.second->get_value() == CV[c.first->get_type()]) {
                    flag = true;
                    value = i.second->get_value();
                    if (i.second->get_input_cell_port_size() == 0) {
                        total_delay = 0;
                        input_transition = 0;
                    }
                    for (auto j: i.second->get_input_cell_port()) {
                        total_delay = j.first->get_total_delay() + this->wire_delay;
                        input_transition = j.first->get_output_transition();
                    }
                }
                else if (!flag) {
                    value = i.second->get_value();
                    for (auto j: i.second->get_input_cell_port()) {
                        if (j.first->get_total_delay() + this->wire_delay > total_delay) {
                            total_delay = j.first->get_total_delay() + this->wire_delay;
                            input_transition = j.first->get_output_transition();
                        }
                        else if (j.first->get_total_delay() + this->wire_delay == total_delay) {
                            if (j.first->get_output_transition() > input_transition) {
                                input_transition = j.first->get_output_transition();
                            }
                        }
                    }
                }
            }
            value = (c.first->get_inversion()) ? INV_TABLE[value] : value;
            switch (value) {
                case FALL:
                    c.first->set_delay(this->table_now->get_table_value("cell_fall", c.first->get_model_name(), input_transition, c.first->get_output_capacitance()));
                    c.first->set_total_delay(total_delay + c.first->get_delay());
                    if (c.first->get_transition() != FALL) {
                        c.first->add_toggle_time_1_0();
                        c.first->set_transition(FALL);
                        this->total_power += c.first->get_switching_power();
                    }
                    c.first->set_output_transition(this->table_now->get_table_value("fall_transition", c.first->get_model_name(), input_transition, c.first->get_output_capacitance()));
                    c.first->set_internal_power(this->table_now->get_table_value("fall_power", c.first->get_model_name(), input_transition, c.first->get_output_capacitance()));
                    this->total_power += c.first->get_internal_power();
                    break;
                case RISE:
                    c.first->set_delay(this->table_now->get_table_value("cell_rise", c.first->get_model_name(), input_transition, c.first->get_output_capacitance()));
                    c.first->set_total_delay(total_delay + c.first->get_delay());
                    if (c.first->get_transition() != RISE) {
                        c.first->add_toggle_time_0_1();
                        c.first->set_transition(RISE);
                        this->total_power += c.first->get_switching_power();
                    }
                    c.first->set_output_transition(this->table_now->get_table_value("rise_transition", c.first->get_model_name(), input_transition, c.first->get_output_capacitance()));
                    c.first->set_internal_power(this->table_now->get_table_value("rise_power", c.first->get_model_name(), input_transition, c.first->get_output_capacitance()));
                    this->total_power += c.first->get_internal_power();
                    break;
                default:
                    break;
            }
            for (auto i: c.first->get_output_port_wire()) {
                i.second->set_value(value);
            }
        }
    }
}

void static_analyzer::find_closing_pair(std::string& str, std::string& content) {
    content = "";
    std::string opening = "([{";
    std::string closing = ")]}";
    int brace_count = 1;
    for (auto c: str) {
        if (opening.find(c) != std::string::npos) {
            ++brace_count;
        }
        else if (closing.find(c) != std::string::npos) {
            --brace_count;
        }
        if (brace_count <= 0) {
            str = str.substr(content.length() + 1);
            break;
        }
        content = content + c;
    }
}
