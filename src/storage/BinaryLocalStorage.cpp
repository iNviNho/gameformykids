#include "BinaryLocalStorage.h"
#include <fstream>


void BinaryLocalStorage::SetMultiple(const std::vector<Coordinate>& coordinates) {
    std::ofstream file(filename, std::ios::binary);

    for (const auto &coordinate : coordinates) {
        file.write(reinterpret_cast<const char *>(&coordinate), sizeof(Coordinate));
    } 
}

std::vector<Coordinate> BinaryLocalStorage::GetAll() {
    std::vector<Coordinate> coordinates;

    std::ifstream file(filename, std::ios::binary);

    Coordinate coordinate;

    while (file.read(
        reinterpret_cast<char*>(&coordinate),
        sizeof(coordinate)
    )) {
        coordinates.push_back(coordinate);  
    }
    return coordinates;
}

