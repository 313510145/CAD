#include "static_analyzer.h"
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <input_lib> <input_netlist> <input_pattern> <output_directory>/\n";
        exit(-1);
    }
    std::ifstream input_file(argv[1]);
    if (!input_file) {
        std::cout << "Cannot open the input file: " << argv[1] << std::endl;
        exit(-1);
    }
    static_analyzer* sa = new static_analyzer();
    sa->input_lib(input_file);
    input_file.close();
    input_file.open(argv[2]);
    if (!input_file) {
        std::cout << "Cannot open the input file: " << argv[2] << std::endl;
        delete sa;
        exit(-1);
    }
    sa->input_verilog(input_file);
    input_file.close();
    std::string lib_name = argv[1];
    size_t last_slash = lib_name.rfind('/');
    if (last_slash != std::string::npos) {
        lib_name = lib_name.substr(last_slash + 1);
    }
    size_t last_dot = lib_name.rfind('.');
    if (last_dot != std::string::npos) {
        lib_name.resize(last_dot);
    }
    std::string file_name = argv[2];
    last_slash = file_name.rfind('/');
    if (last_slash != std::string::npos) {
        file_name = file_name.substr(last_slash + 1);
    }
    last_dot = file_name.rfind('.');
    if (last_dot != std::string::npos) {
        file_name.resize(last_dot);
    }
    std::string output_name = argv[4] + lib_name + "_" + file_name + ".cap";
    std::ofstream output_file(output_name);
    if (!output_file) {
        std::cout << "Cannot open the output file: " << output_name << std::endl;
        delete sa;
        exit(-1);
    }
    sa->calculate_and_output_capacitance(output_file);
    output_file.close();
    input_file.open(argv[3]);
    if (!input_file) {
        std::cout << "Cannot open the input file: " << argv[3] << std::endl;
        delete sa;
        exit(-1);
    }
    sa->input_pattern(input_file);
    input_file.close();
    output_name = argv[4] + lib_name + "_" + file_name + ".del";
    std::string output_power_name = argv[4] + lib_name + "_" + file_name + ".pow";
    std::string output_toggle_name = argv[4] + lib_name + "_" + file_name + ".tog";
    output_file.open(output_name);
    if (!output_file) {
        std::cout << "Cannot open the output file: " << output_name << std::endl;
        delete sa;
        exit(-1);
    }
    std::ofstream output_power(output_power_name);
    if (!output_power) {
        std::cout << "Cannot open the output file: " << output_power_name << std::endl;
        delete sa;
        exit(-1);
    }
    std::ofstream output_toggle(output_toggle_name);
    if (!output_toggle) {
        std::cout << "Cannot open the output file: " << output_toggle_name << std::endl;
        delete sa;
        exit(-1);
    }
    sa->calculate_and_output_delay_power_toggle(output_file, output_power, output_toggle);
    output_file.close();
    output_power.close();
    output_toggle.close();
    delete sa;
    return 0;
}
