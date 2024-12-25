#ifndef MINIMUM_COVER_FINDER_H
#define MINIMUM_COVER_FINDER_H

#include <iostream>
#include <vector>
#include <string>
#include <set>

class minimum_cover_finder {
    public:
        void input_information(std::istream& is);
        void output_prime_implicants(std::ostream& os);
        minimum_cover_finder();
        ~minimum_cover_finder();
    private:
        void run();
        static void expansion(std::vector<std::set<std::string>>& sop, const std::vector<std::string>& sum);
        unsigned int bit_num;
        unsigned int literal_count;
        std::vector<std::string> on_set;
        std::vector<std::vector<std::pair<std::string, bool>>> implication_table;
        std::set<std::string> prime_implicant;
        std::vector<std::vector<std::string>> product_of_sum;
        std::vector<std::set<std::string>> sum_of_product;
};

#endif  // MINIMUM_COVER_FINDER_H
