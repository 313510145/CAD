#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>
#include <unordered_map>

class table {
    public:
        std::string get_name() const;
        bool get_port_is_output(const std::string& cell_name, const std::string& port_name) const;
        double get_port_capacitance(const std::string& cell_name, const std::string& port_name) const;
        unsigned int get_port_information_size(const std::string& cell_name) const;
        unsigned int get_total_output_net_capacitance_size() const;
        unsigned int get_input_transition_time_size() const;
        double get_table_value(const std::string& type, const std::string& cell_name, const double& input_transition_time_in, const double& total_output_net_capacitance_in) const;
        void set_name(const std::string& name_in);
        void add_port_information(std::string cell_name_in, const std::string& port_name_in, std::pair<bool, double> is_output_capacitance_in);
        void set_index(const std::string& variable, const std::vector<double>& index_values);
        void resize_table();
        void set_table(const std::string& cell_name_in, const std::string& table_type, const std::vector<std::vector<double>>& table_values);
        table();
        ~table();
    private:
        std::string name;
        static std::vector<std::unordered_map<std::string, std::pair<bool, double>>> port_information;
        static std::unordered_map<std::string, int> cell_name_index_map;
        std::unordered_map<std::string, std::vector<double>*> variable_index_map;
        std::unordered_map<std::string, std::vector<std::vector<std::vector<double>>>*> table_type_map;
        std::vector<double> total_output_net_capacitance;
        std::vector<double> input_transition_time;
        std::vector<std::vector<std::vector<double>>> rise_power;
        std::vector<std::vector<std::vector<double>>> fall_power;
        std::vector<std::vector<std::vector<double>>> cell_rise;
        std::vector<std::vector<std::vector<double>>> cell_fall;
        std::vector<std::vector<std::vector<double>>> rise_transition;
        std::vector<std::vector<std::vector<double>>> fall_transition;
};

#endif  // TABLE_H
