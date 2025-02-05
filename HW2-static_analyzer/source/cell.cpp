#include "cell.h"
#include <cfloat>

std::string cell::get_model_name() const {
    return this->model_name;
}

std::string cell::get_name() const {
    return this->name;
}

std::unordered_map<std::string, wire*> cell::get_input_port_wire() const {
    return this->input_port_wire;
}

unsigned int cell::get_input_port_wire_size() const {
    return this->input_port_wire.size();
}

std::unordered_map<std::string, wire*> cell::get_output_port_wire() const {
    return this->output_port_wire;
}

unsigned int cell::get_output_port_wire_size() const {
    return this->output_port_wire.size();
}

int cell::get_type() const {
    return this->type;
}

double cell::get_output_capacitance() const {
    return this->output_capacitance;
}

double cell::get_output_transition() const {
    return this->output_transition;
}

double cell::get_delay() const {
    return this->delay;
}

double cell::get_total_delay() const {
    return this->total_delay;
}

double cell::get_internal_power() const {
    return this->internal_power;
}

double cell::get_switching_power() const {
    return this->switching_power;
}

unsigned int cell::get_toggle_time_0_1() const {
    return this->toggle_time_0_1;
}

unsigned int cell::get_toggle_time_1_0() const {
    return this->toggle_time_1_0;
}

int cell::get_transition() const {
    return this->transition;
}

bool cell::get_flag() const {
    return this->flag;
}

bool cell::get_inversion() const {
    if (
        this->type == NAND ||
        this->type == NOR ||
        this->type == INV
    ) {
        return true;
    }
    return false;
}

void cell::set_model_name(const std::string& model_name_in) {
    this->model_name = model_name_in;
}

void cell::set_name(const std::string& name_in) {
    this->name = name_in;
}

void cell::add_input_port_wire(const std::string& input_port_in, wire* wire_in) {
    this->input_port_wire[input_port_in] = wire_in;
}

void cell::set_output_port_wire(const std::string& output_port_in, wire* wire_in) {
    this->output_port_wire[output_port_in] = wire_in;
}

void cell::set_type(const int& type_in) {
    this->type = type_in;
}

void cell::set_output_capacitance(const double& output_capacitance_in) {
    this->output_capacitance = output_capacitance_in;
}

void cell::set_output_transition(const double& output_transition_in) {
    if (output_transition_in < 0) {
        this->output_transition = 0;
        return;
    }
    this->output_transition = output_transition_in;
}

void cell::set_delay(const double& delay_in) {
    if (delay_in < 0) {
        this->delay = 0;
        return;
    }
    this->delay = delay_in;
}

void cell::set_total_delay(const double& total_delay_in) {
    this->total_delay = total_delay_in;
}

void cell::set_internal_power(const double& internal_power_in) {
    if (internal_power_in < 0) {
        this->internal_power = 0;
        return;
    }
    this->internal_power = internal_power_in;
}

void cell::set_switching_power(const double& switching_power_in) {
    this->switching_power = switching_power_in;
}

void cell::add_toggle_time_0_1() {
    ++this->toggle_time_0_1;
}

void cell::add_toggle_time_1_0() {
    ++this->toggle_time_1_0;
}

void cell::reset_toggle_time_0_1() {
    this->toggle_time_0_1 = 0;
}

void cell::reset_toggle_time_1_0() {
    this->toggle_time_1_0 = 0;
}

void cell::set_transition(const int& transition_in) {
    this->transition = transition_in;
}

void cell::set_flag() {
    this->flag = true;
}

void cell::reset_flag() {
    this->flag = false;
}

cell::cell() {
    this->model_name = "";
    this->name = "";
    this->type = BAD_CELL;
    this->output_capacitance = 0;
    this->output_transition = DBL_MAX;
    this->delay = DBL_MAX;
    this->total_delay = DBL_MAX;
    this->internal_power = 0;
    this->switching_power = 0;
    this->toggle_time_0_1 = 0;
    this->toggle_time_1_0 = 0;
    this->transition = UNDEFINED;
    this->flag = false;
}

cell::~cell() {}
