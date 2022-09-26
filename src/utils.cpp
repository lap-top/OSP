#include "utils.hpp"


void validateInput(std::ifstream* file, std::string filename) {
    if (!file->is_open() || file->fail()) {
        std::cerr << "ERROR: Failed Opening: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
}


void validateOutput(std::ofstream* file, std::string filename) {
    if (!file->is_open() || file->fail()) {
        std::cerr << "ERROR: Failed Opening: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
}


bool isAlpha(const std::string &str) {
    auto it = std::find_if(str.begin(), str.end(), [](char const &c) {
            return !isalpha(c);
        });
    return it == str.end();
}


bool cmpOnThirdChar(std::string str1, std::string str2) {
    return str1.substr(2, str1.length()) < str2.substr(2, str2.length());
} 


bool lexicalCompare(std::string str1, std::string str2) {
    return str1 < str2;
}