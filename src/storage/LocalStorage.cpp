#include "LocalStorage.h"

#include <fstream>

void LocalStorage::persist(const std::string &data) {
    // open file in append mode
    std::ofstream file(filename, std::ios::app);
    if (!file) {                        // check if opened successfully
        throw std::runtime_error("Can't open local storage file for writing: " + filename);
    }
    // write data
    file << data;
    // close file
    file.close();
}

std::vector<std::string> LocalStorage::GetLines() {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Can't open local storage file for reading: " + filename);
    }
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}