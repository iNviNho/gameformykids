#include "TerrainHeight.h"
#include <vector>

void TerrainHeight::SetMultiple(const std::vector<Coordinate>& coordinates) {
    // we must update internal data of the coordinates
    for (const auto& value: coordinates) {
        // we negative Z coordinate because it must be positive so that uniqueness works
         data[value.z * size * -1 + value.x] = value.y;
    }

    // as well as persist that change into the storage
    storage.SetMultiple(coordinates);
}

void TerrainHeight::loadDataFromStorage() {
    for (const auto& coordinate: storage.GetAll()) {
        // we negative Z coordinate because it must be positive so that uniqueness works
        data[coordinate.z * size * -1 + coordinate.x] = coordinate.y;
    } 
}

float TerrainHeight::Get(int x, int z) const {
    auto it = data.find(z * size * -1 + x);
    return it == data.end() ? 0.0f: it->second;
}; 
