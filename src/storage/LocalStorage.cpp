#include "LocalStorage.h"

#include "../utils/Log.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

void LocalStorage::insert(const std::string& key, const std::string& value) {
    std::ifstream fileToRead(filename);
    std::string line;
    std::unordered_map<std::string, std::string> data;

    while (std::getline(fileToRead, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Find the '=' separator
        std::size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            // Skip lines without '='
            continue;
        }
        std::string existingKey = line.substr(0, equalPos);
        std::string existingValue = line.substr(equalPos + 1);

        data[existingKey] = existingValue;
    }

    fileToRead.close();
    
    // write new value
    data[key] = value;

    // open file in truncate mode
    std::ofstream fileToWrite(filename);
    if (!fileToWrite) {
        throw std::runtime_error("Can't open local storage file for writing: " + filename);
    }
    if (key.find("=") != std::string::npos) {
        throw std::runtime_error("Key must not contain = character");
    }
    
    for (const auto& [keyToWrite, valueToWrite] : data) {
        // write key;value to the file
        fileToWrite << keyToWrite << "=" << valueToWrite << '\n';
    }
    // close file 
    fileToWrite.close();
}

void LocalStorage::insertMultiple(const std::unordered_map<std::string, std::string>& newData) {
    std::ifstream fileToRead(filename);
    std::string line;
    std::unordered_map<std::string, std::string> data;

    // Load existing data
    while (std::getline(fileToRead, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }

        data.emplace(
            line.substr(0, equalPos),
            line.substr(equalPos + 1)
        );
    }

    fileToRead.close();

    // Validate + merge new data
    for (const auto& [k, v] : newData) {
        if (k.find('=') != std::string::npos) {
            throw std::runtime_error("Key must not contain = character: " + k);
        }

        data.insert_or_assign(k, v);
    }

    // Write everything back
    std::ofstream fileToWrite(filename, std::ios::trunc);
    if (!fileToWrite) {
        throw std::runtime_error("Can't open local storage file for writing: " + filename);
    }

    std::string out;
    out.reserve(data.size() * 24); // rough guess
    
    for (const auto& [k, v] : data) {
        out += k;
        out += '=';
        out += v;
        out += '\n';
    }
    fileToWrite.write(out.data(), out.size());
}

std::unordered_map<std::string, std::string> LocalStorage::GetAll() {
    std::unordered_map<std::string, std::string> data;
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Can't open local storage file for reading: " + filename);
    }
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Find the '=' separator
        std::size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            // Skip lines without '='
            continue;
        }
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        // we delete \r, \n and space 
        value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
        data[key] = value;
    }
    return data;
}

std::string LocalStorage::getOne(const std::string& key, const std::string& defaultValue) {
    auto keyValueMap = GetAll();
    auto it = keyValueMap.find(key);
    if (it != keyValueMap.end()) {
        return it->second;
    }
    return defaultValue;
}

void LocalStorage::remove(const std::string& key) {
    std::ifstream fileToRead(filename);
    std::string line;
    std::unordered_map<std::string, std::string> data;

    while (std::getline(fileToRead, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        // Find the '=' separator
        std::size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            // Skip lines without '='
            continue;
        }
        std::string existingKey = line.substr(0, equalPos);
        std::string existingValue = line.substr(equalPos + 1);

        if (existingKey != key) {
            data[existingKey] = existingValue;
        }
    }

    fileToRead.close();

    // open file in truncate mode
    std::ofstream fileToWrite(filename);
    if (!fileToWrite) {
        throw std::runtime_error("Can't open local storage file for writing: " + filename);
    }
    if (key.find("=") != std::string::npos) {
        throw std::runtime_error("Key must not contain = character");
    }
    
    for (const auto& [keyToWrite, valueToWrite] : data) {
        // write key;value to the file
        fileToWrite << keyToWrite << "=" << valueToWrite << '\n';
    }
    // close file 
    fileToWrite.close();
}

std::vector<std::string> LocalStorage::split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string part;

    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}
