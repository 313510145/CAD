#ifndef WIRE_H
#define WIRE_H

#include "cell.h"
#include <string>
#include <unordered_map>

enum VALUE {S0, S1, X};

class cell;

class wire {
    public:
        std::string get_name() const;
        std::unordered_map<cell*, std::string> get_input_cell_port() const;
        unsigned int get_input_cell_port_size() const;
        std::unordered_map<cell*, std::string> get_output_cell_port() const;
        unsigned int get_output_cell_port_size() const;
        int get_value() const;
        void set_name(const std::string& name_in);
        void set_input_cell_port(cell* input_cell_in, const std::string& port_in);
        void add_output_cell_port(cell* output_cell_in, const std::string& port_in);
        void set_value(const int& value_in);
        wire();
        ~wire();
    private:
        std::string name;
        std::unordered_map<cell*, std::string> input_cell_port;
        std::unordered_map<cell*, std::string> output_cell_port;
        int value;
};

#endif  // WIRE_H
