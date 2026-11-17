#include "TerrainHeight.h"
#include "../utils/Log.h"
#include <cmath>
#include <string>
#include <vector>

void TerrainHeight::SetMultiple(const std::vector<Coordinate>& coordinates) {
    std::vector<Coordinate> validCoordinates;
    validCoordinates.reserve(coordinates.size());
    for (const auto& value : coordinates) {
        if (value.x < 0 || value.x >= size || value.z > 0 || value.z <= -size || !std::isfinite(value.y)) {
            Log::logWarning("Skipping invalid terrain height update: x=" + std::to_string(value.x) +
                            ", z=" + std::to_string(value.z) + ", y=" + std::to_string(value.y));
            continue;
        }
        const int key = (-value.z) * size + value.x;
        data[key] = value.y;
        validCoordinates.push_back(value);
    }

    // Persist only this brush update. BinaryLocalStorage merges it with the
    // existing records so untouched terrain edits are retained.
    storage.SetMultiple(validCoordinates);
}

void TerrainHeight::loadDataFromStorage() {
    for (const auto& coordinate : storage.GetAll()) {
        // we negative Z coordinate because it must be positive so that uniqueness works
        data[coordinate.z * size * -1 + coordinate.x] = coordinate.y;
    }
}

float TerrainHeight::Get(int x, int z) const {
    auto it = data.find(z * size * -1 + x);
    return it == data.end() ? 0.0f: it->second;
}; 
