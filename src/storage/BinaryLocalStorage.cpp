#include "BinaryLocalStorage.h"
#include "../utils/Log.h"
#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace {
constexpr std::uintmax_t RECORD_SIZE = sizeof(std::int32_t) * 2 + sizeof(float);
}

void BinaryLocalStorage::SetMultiple(const std::vector<Coordinate>& coordinates) {
    // Edits are sparse, so append them. Remove only a torn final record first;
    // rewriting the whole file would be unnecessary and risks losing other edits.
    std::error_code error;
    const auto existingSize = std::filesystem::file_size(filename, error);
    if (!error && existingSize % RECORD_SIZE != 0) {
        const auto completeSize = existingSize - (existingSize % RECORD_SIZE);
        std::filesystem::resize_file(filename, completeSize, error);
        if (error) {
            throw std::runtime_error("Can't remove incomplete terrain storage record from: " + filename);
        }
        Log::logWarning("Removed incomplete trailing terrain storage record from: " + filename);
    }

    std::ofstream file(filename, std::ios::binary | std::ios::app);
    if (!file) {
        throw std::runtime_error("Can't open terrain storage file for writing: " + filename);
    }
    for (const auto& coordinate : coordinates) {
        file.write(reinterpret_cast<const char*>(&coordinate.x), sizeof(coordinate.x));
        file.write(reinterpret_cast<const char*>(&coordinate.z), sizeof(coordinate.z));
        file.write(reinterpret_cast<const char*>(&coordinate.y), sizeof(coordinate.y));
        if (!file) {
            throw std::runtime_error("Failed while writing terrain storage file: " + filename);
        }
    }
}

std::vector<Coordinate> BinaryLocalStorage::GetAll() {
    std::vector<Coordinate> coordinates;

    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        // A missing storage file means a new, flat terrain.
        return coordinates;
    }

    while (true) {
        Coordinate coordinate{};
        if (!file.read(reinterpret_cast<char*>(&coordinate.x), sizeof(coordinate.x))) {
            if (file.gcount() != 0) {
                Log::logWarning("Ignoring incomplete trailing terrain storage record in: " + filename);
            }
            break;
        }
        if (!file.read(reinterpret_cast<char*>(&coordinate.z), sizeof(coordinate.z)) ||
            !file.read(reinterpret_cast<char*>(&coordinate.y), sizeof(coordinate.y))) {
            Log::logWarning("Ignoring incomplete trailing terrain storage record in: " + filename);
            break;
        }
        coordinates.push_back(coordinate);  
    }
    return coordinates;
}

