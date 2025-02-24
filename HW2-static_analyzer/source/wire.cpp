#include "wire.h"

std::string wire::get_name() const {
    return this->name;
}

std::unordered_map<cell*, std::string> wire::get_input_cell_port() const {
    return this->input_cell_port;
}

unsigned int wire::get_input_cell_port_size() const {
    return this->input_cell_port.size();
}

std::unordered_map<cell*, std::string> wire::get_output_cell_port() const {
    return this->output_cell_port;
}

unsigned int wire::get_output_cell_port_size() const {
    return this->output_cell_port.size();
}

int wire::get_value() const {
    return this->value;
}

void wire::set_name(const std::string& name_in) {
    this->name = name_in;
}

void wire::set_input_cell_port(cell* input_cell_in, const std::string& port_in) {
    this->input_cell_port[input_cell_in] = port_in;
}

void wire::add_output_cell_port(cell* output_cell_in, const std::string& port_in) {
    this->output_cell_port[output_cell_in] = port_in;
}

void wire::set_value(const int& value_in) {
    this->value = value_in;
}

wire::wire() {
    this->name = "";
    this->value = UNDEFINED;
}

wire::~wire() {}
