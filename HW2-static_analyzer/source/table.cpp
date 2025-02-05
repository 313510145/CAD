#include "table.h"

std::vector<std::unordered_map<std::string, std::pair<bool, double>>> table::port_information;
std::unordered_map<std::string, int> table::cell_name_index_map;

std::string table::get_name() const {
    return this->name;
}

bool table::get_port_is_output(const std::string& cell_name, const std::string& port_name) const {
    return this->port_information[this->cell_name_index_map.at(cell_name)].at(port_name).first;
}

double table::get_port_capacitance(const std::string& cell_name, const std::string& port_name) const {
    return this->port_information[this->cell_name_index_map.at(cell_name)].at(port_name).second;
}

unsigned int table::get_port_information_size(const std::string& cell_name) const {
    return this->port_information[this->cell_name_index_map.at(cell_name)].size();
}

unsigned int table::get_total_output_net_capacitance_size() const {
    return this->total_output_net_capacitance.size();
}

double table::get_table_value(const std::string& type, const std::string& cell_name, const double& input_transition_time_in, const double& total_output_net_capacitance_in) const {
    const std::vector<std::vector<double>>* t = &(this->table_type_map.at(type)->at(this->cell_name_index_map.at(cell_name)));
    unsigned int x1, x2, y1, y2;
    for (x2 = 1; x2 < this->input_transition_time.size() - 1; x2++) {
        if (this->input_transition_time[x2] >= input_transition_time_in) {
            break;
        }
    }
    x1 = x2 - 1;
    for (y2 = 1; y2 < this->total_output_net_capacitance.size() - 1; y2++) {
        if (this->total_output_net_capacitance[y2] >= total_output_net_capacitance_in) {
            break;
        }
    }
    y1 = y2 - 1;
    double q11 = (*t)[x1][y1];
    double q12 = (*t)[x1][y2];
    double q21 = (*t)[x2][y1];
    double q22 = (*t)[x2][y2];
    double q11_12 = q11 + (q12 - q11) * (total_output_net_capacitance_in - this->total_output_net_capacitance[y1]) / (this->total_output_net_capacitance[y2] - this->total_output_net_capacitance[y1]);
    double q21_22 = q21 + (q22 - q21) * (total_output_net_capacitance_in - this->total_output_net_capacitance[y1]) / (this->total_output_net_capacitance[y2] - this->total_output_net_capacitance[y1]);
    double result = q11_12 + (q21_22 - q11_12) * (input_transition_time_in - this->input_transition_time[x1]) / (this->input_transition_time[x2] - this->input_transition_time[x1]);
    if (result < 0) {
        return 0;
    }
    return result;
}

void table::set_name(const std::string& name_in) {
    this->name = name_in;
}

void table::add_port_information(std::string cell_name_in, const std::string& port_name_in, std::pair<bool, double> is_output_capacitance_in) {
    this->cell_name_index_map.emplace(cell_name_in, this->cell_name_index_map.size());
    this->port_information.resize(this->cell_name_index_map.size());
    this->port_information[this->cell_name_index_map[cell_name_in]][port_name_in] = is_output_capacitance_in;
}

void table::set_index(const std::string& variable, const std::vector<double>& index_values) {
    *(this->variable_index_map[variable]) = index_values;
}

void table::resize_table() {
    this->rise_power.resize(this->cell_name_index_map.size());
    this->fall_power.resize(this->cell_name_index_map.size());
    this->cell_rise.resize(this->cell_name_index_map.size());
    this->cell_fall.resize(this->cell_name_index_map.size());
    this->rise_transition.resize(this->cell_name_index_map.size());
    this->fall_transition.resize(this->cell_name_index_map.size());
}

void table::set_table(const std::string& cell_name_in, const std::string& table_type, const std::vector<std::vector<double>>& table_values) {
    (*(this->table_type_map[table_type]))[this->cell_name_index_map[cell_name_in]] = table_values;
}

table::table() {
    this->name = "";
    this->variable_index_map["total_output_net_capacitance"] = &this->total_output_net_capacitance;
    this->variable_index_map["input_transition_time"] = &this->input_transition_time;
    this->table_type_map["rise_power"] = &this->rise_power;
    this->table_type_map["fall_power"] = &this->fall_power;
    this->table_type_map["cell_rise"] = &this->cell_rise;
    this->table_type_map["cell_fall"] = &this->cell_fall;
    this->table_type_map["rise_transition"] = &this->rise_transition;
    this->table_type_map["fall_transition"] = &this->fall_transition;
}

table::~table() {}
