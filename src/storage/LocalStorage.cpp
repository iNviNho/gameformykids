#include "LocalStorage.h"

#include <fstream>
#include <sstream>
#include <vector>

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

bool LocalStorage::RemoveEntityByPosition(const glm::vec3& pos, float epsilon) {
    // read all lines
    std::vector<std::string> lines = GetLines();
    if (lines.empty()) return false;

    const float epsSq = epsilon * epsilon;
    std::vector<std::string> keep;
    keep.reserve(lines.size());

    bool anyRemoved = false;
    for (const auto &line : lines) {
        // expected format: name,unique_id,entity.toString()
        // entity.toString() -> posx,posy,posz,rotx,roty,rotz,scale
        std::size_t firstComma = line.find(',');
        if (firstComma == std::string::npos) {
            keep.push_back(line);
            continue;
        }
        std::size_t secondComma = line.find(',', firstComma + 1);
        if (secondComma == std::string::npos) {
            keep.push_back(line);
            continue;
        }
        std::string entityData = line.substr(secondComma + 1);
        // split by comma
        std::istringstream iss(entityData);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
            if (tokens.size() >= 3) break; // only need first three (pos)
        }
        if (tokens.size() < 3) {
            keep.push_back(line);
            continue;
        }
        try {
            const float x = std::stof(tokens[0]);
            const float y = std::stof(tokens[1]);
            const float z = std::stof(tokens[2]);
            const float dx = x - pos.x;
            const float dy = y - pos.y;
            const float dz = z - pos.z;
            const float distSq = dx*dx + dy*dy + dz*dz;
            if (distSq <= epsSq) {
                anyRemoved = true;
                continue; // skip (remove) this line
            }
        } catch (...) {
            // parsing failed -> keep the line
            keep.push_back(line);
            continue;
        }
        // keep line
        keep.push_back(line);
    }

    if (!anyRemoved) return false;

    // write back kept lines (truncate file)
    std::ofstream out(filename, std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Can't open local storage file for writing: " + filename);
    }
    for (const auto &k : keep) {
        out << k << '\n';
    }
    out.close();

    return true;
}
