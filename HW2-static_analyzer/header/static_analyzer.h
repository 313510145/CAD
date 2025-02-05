#ifndef STATIC_ANALYZER_H
#define STATIC_ANALYZER_H

#include "cell.h"
#include "wire.h"
#include "table.h"
#include <iostream>

class static_analyzer {
    public:
        void input_lib(std::istream& is);
        void input_verilog(std::istream& is);
        void input_pattern(std::istream& is);
        void calculate_and_output_capacitance(std::ostream& os);
        void calculate_and_output_delay_power_toggle(std::ostream& os_delay, std::ostream& os_power, std::ostream& os_toggle);
        static_analyzer();
        ~static_analyzer();
    private:
        void pre_processing_lib(std::string& str);
        void pre_processing_verilog(std::string& str);
        void recursive_wire(const wire* w);
        std::string name;
        std::pair<double, std::string> time_unit, voltage_unit, current_unit, capacitive_load_unit;
        std::vector<table*> table_list;
        table* table_now;
        std::vector<cell*> cell_list;
        std::vector<wire*> wire_list;
        std::vector<wire*> PI_list;
        std::vector<wire*> PO_list;
        std::unordered_map<std::string, wire*> name_to_wire;
        std::unordered_map<std::string, table*> name_to_table;
        std::unordered_map<std::string, int> name_to_cell_type;
        std::vector<std::vector<int>> pattern;
        double PO_capacitance, wire_delay, operation_voltage;
        double total_power;
        unsigned int max_toggle_time;
};

#endif  // STATIC_ANALYZER_H
