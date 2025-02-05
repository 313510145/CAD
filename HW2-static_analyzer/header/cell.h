#ifndef CELL_H
#define CELL_H

#include "wire.h"
#include <string>
#include <unordered_map>

enum TRANSITION_TYPE {FALL, RISE, UNDEFINED};
enum CELL_TYPE {NAND, NOR, INV, BAD_CELL};
const std::string CELL_TYPE_NAME[] = {"NAND", "NOR", "INV"};
const TRANSITION_TYPE CV[] = {FALL, RISE, UNDEFINED};
const TRANSITION_TYPE INV_TABLE[] = {RISE, FALL, UNDEFINED};

class wire;

class cell {
    public:
        std::string get_model_name() const;
        std::string get_name() const;
        std::unordered_map<std::string, wire*> get_input_port_wire() const;
        unsigned int get_input_port_wire_size() const;
        std::unordered_map<std::string, wire*> get_output_port_wire() const;
        unsigned int get_output_port_wire_size() const;
        int get_type() const;
        double get_output_capacitance() const;
        double get_output_transition() const;
        double get_delay() const;
        double get_total_delay() const;
        double get_internal_power() const;
        double get_switching_power() const;
        unsigned int get_toggle_time_0_1() const;
        unsigned int get_toggle_time_1_0() const;
        int get_transition() const;
        bool get_flag() const;
        bool get_inversion() const;
        void set_model_name(const std::string& model_name_in);
        void set_name(const std::string& name_in);
        void add_input_port_wire(const std::string& input_port_in, wire* wire_in);
        void set_output_port_wire(const std::string& output_port_in, wire* wire_in);
        void set_type(const int& type_in);
        void set_output_capacitance(const double& output_capacitance_in);
        void set_output_transition(const double& output_transition_in);
        void set_delay(const double& delay_in);
        void set_total_delay(const double& total_delay_in);
        void set_internal_power(const double& internal_power_in);
        void set_switching_power(const double& switching_power_in);
        void add_toggle_time_0_1();
        void add_toggle_time_1_0();
        void reset_toggle_time_0_1();
        void reset_toggle_time_1_0();
        void set_transition(const int& transition_in);
        void set_flag();
        void reset_flag();
        cell();
        ~cell();
    private:
        std::string model_name;
        std::string name;
        std::unordered_map<std::string, wire*> input_port_wire;
        std::unordered_map<std::string, wire*> output_port_wire;
        int type;
        double output_capacitance, output_transition;
        double delay, total_delay;
        double internal_power, switching_power;
        unsigned int toggle_time_0_1, toggle_time_1_0;
        int transition;
        bool flag;
};

#endif  // CELL_H
