#include "minimum_cover_finder.h"
#include <algorithm>

void minimum_cover_finder::input_information(std::istream& is) {
    std::string temp_str;
    unsigned int minterm, one_count;
    bool temp;
    is >> temp_str >> this->bit_num;
    this->implication_table.resize(this->bit_num + 1);
    is >> temp_str >> temp_str;
    while (temp_str != ".d") {
        minterm = stoi(temp_str);
        temp_str = "";
        one_count = 0;
        for (unsigned int i = this->bit_num - 1; i < this->bit_num ; i--) {
            temp = (minterm >> i) & 1;
            if (temp) {
                one_count++;
                temp_str += "1";
            }
            else {
                temp_str += "0";
            }
        }
        this->on_set.push_back(temp_str);
        this->implication_table[one_count].push_back(make_pair(temp_str, true));
        is >> temp_str;
    }
    while (is >> temp_str) {
        minterm = stoi(temp_str);
        temp_str = "";
        one_count = 0;
        for (unsigned int i = this->bit_num - 1; i < this->bit_num ; i--) {
            temp = (minterm >> i) & 1;
            if (temp) {
                one_count++;
                temp_str += "1";
            }
            else {
                temp_str += "0";
            }
        }
        this->implication_table[one_count].push_back(make_pair(temp_str, true));
    }
    run();
}

void minimum_cover_finder::output_prime_implicants(std::ostream& os) {
    unsigned int temp_count = 0;
    os << ".p " << this->prime_implicant.size() << std::endl;
    for (std::set<std::string>::reverse_iterator it = this->prime_implicant.rbegin(); it != this->prime_implicant.rend() && temp_count < 15; ++it) {
        for (unsigned int i = 0; i < this->bit_num; i++) {
            if ((*it)[i] == '0') {
                os << static_cast<char>('A' + i) << "'";
            }
            else if ((*it)[i] == '1') {
                os << static_cast<char>('A' + i);
            }
        }
        temp_count++;
        os << "\n";
    }
    os << "\n"
       << ".mc " << this->sum_of_product[0].size() << std::endl;
    for (std::set<std::string>::reverse_iterator it = this->sum_of_product[0].rbegin(); it != this->sum_of_product[0].rend(); ++it) {
        for (unsigned int i = 0; i < this->bit_num; i++) {
            if ((*it)[i] == '0') {
                os << static_cast<char>('A' + i) << "'";
            }
            else if ((*it)[i] == '1') {
                os << static_cast<char>('A' + i);
            }
        }
        os << "\n";
    }
    os << "literal=" << this->literal_count << std::endl;
}

void minimum_cover_finder::run() {
    std::pair<std::string, bool> temp_implicant;
    temp_implicant.second = true;
    unsigned int diff_count;
    std::vector<std::pair<std::string, bool>> temp_vector;
    for (unsigned int r = 0; r < this->bit_num; r++) {
        for (unsigned int i = 0; i < this->implication_table.size() - r - 1; i++) {
            for (auto& j: this->implication_table[i]) {
                for (auto& k: this->implication_table[i + 1]) {
                    temp_implicant.first = "";
                    diff_count = 0;
                    for (unsigned int l = 0; l < this->bit_num; l++) {
                        if (j.first[l] == k.first[l]) {
                            temp_implicant.first += j.first[l];
                        }
                        else {
                            temp_implicant.first += "-";
                            diff_count++;
                        }
                    }
                    if (diff_count == 1) {
                        if (std::find(temp_vector.begin(), temp_vector.end(), temp_implicant) == temp_vector.end()) {
                            temp_vector.push_back(temp_implicant);
                        }
                        j.second = false;
                        k.second = false;
                    }
                }
                if (j.second == true) {
                    this->prime_implicant.insert(j.first);
                }
            }
            this->implication_table[i] = temp_vector;
            temp_vector.clear();
        }
    }
    for (unsigned int i = 0; i < this->implication_table.size(); i++) {
        for (auto j: this->implication_table[i]) {
            if (j.second == true) {
                this->prime_implicant.insert(j.first);
            }
        }
    }
    std::vector<std::string> sum;
    for (const auto& i: this->on_set) {
        for (auto j: this->prime_implicant) {
            for (unsigned int k = 0; k < this->bit_num; k++) {
                if ((j[k] != '-') && (j[k] != i[k])) {
                    temp_implicant.second = false;
                    break;
                }
            }
            if (temp_implicant.second == true) {
                sum.push_back(j);
            }
            temp_implicant.second = true;
        }
        this->product_of_sum.push_back(sum);
        sum.clear();
    }
    for (auto i: this->product_of_sum) {
        if (i.size() == 1) {
            sum.push_back(i[0]);
        }
    }
    for (auto i: sum) {
        for (std::vector<std::vector<std::string>>::iterator it = this->product_of_sum.begin(); it != this->product_of_sum.end();) {
            if (std::find(it->begin(), it->end(), i) != it->end() && it->size() != 1) {
                this->product_of_sum.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    for (auto i: this->product_of_sum[0]) {
        std::set<std::string> temp_set({i});
        this->sum_of_product.push_back(temp_set);
    }
    for (unsigned int i = 1; i < this->product_of_sum.size(); i++) {
        expansion(this->sum_of_product, this->product_of_sum[i]);
    }
    for (unsigned int i = 0; i < this->sum_of_product.size(); i++) {
        unsigned int temp_count = 0;
        for (auto j: this->sum_of_product[i]) {
            temp_count += count(j.begin(), j.end(), '0');
            temp_count += count(j.begin(), j.end(), '1');
        }
        if (temp_count < this->literal_count) {
            this->literal_count = temp_count;
            diff_count = i;
        }
    }
    std::set<std::string> temp_set = this->sum_of_product[diff_count];
    this->sum_of_product[diff_count] = this->sum_of_product[0];
    this->sum_of_product[0] = temp_set;
}

void minimum_cover_finder::expansion(std::vector<std::set<std::string>>& sop, const std::vector<std::string>& sum) {
    std::vector<std::set<std::string>> new_sop, temp_sop;
    unsigned int min_size = 0xFFFFFFFF;
    for (const auto& i: sop) {
        for (auto j: sum) {
            std::set<std::string> temp = i;
            temp.insert(j);
            min_size = (temp.size() < min_size) ? temp.size() : min_size;
            temp_sop.push_back(temp);
        }
    }
    std::copy_if(
        temp_sop.begin(),
        temp_sop.end(),
        std::back_inserter(new_sop),
        [&min_size](const auto& i) {
            return i.size() == min_size;
        }
    );
    sop = new_sop;
}

minimum_cover_finder::minimum_cover_finder() {
    this->bit_num = 0;
    this->literal_count = 0xFFFFFFFF;
}

minimum_cover_finder::~minimum_cover_finder() {
    this->on_set.clear();
    this->prime_implicant.clear();
    for (auto& i: this->implication_table) {
        i.clear();
    }
    for (auto& i: this->product_of_sum) {
        i.clear();
    }
    for (auto& i: this->sum_of_product) {
        i.clear();
    }
    this->implication_table.clear();
    this->product_of_sum.clear();
    this->sum_of_product.clear();
}
