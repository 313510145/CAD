#include "minimum_cover_finder.h"
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        exit(-1);
    }
    std::ifstream input_file(argv[1]);
    if (!input_file) {
        std::cout << "Cannot open the input file: " << argv[1] << std::endl;
        exit(-1);
    }
    minimum_cover_finder* mcf = new minimum_cover_finder();
    mcf->input_information(input_file);
    input_file.close();
    std::ofstream output_file(argv[2]);
    if (!output_file) {
        std::cout << "Cannot open the output file: " << argv[2] << std::endl;
        delete mcf;
        exit(-1);
    }
    mcf->output_prime_implicants(output_file);
    delete mcf;
    output_file.close();
    return 0;
}
