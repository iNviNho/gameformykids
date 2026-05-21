
#include "TerrainHeight.h"
#include <string>
#include <vector>
#include "../tracing/Tracer.h"


void TerrainHeight::loadDataFromStorage() {
    TRACE_ME();
    for (const auto& [key, value] : storage.GetAll()) {
        auto keySplitted = storage.split(key, ',');
        // we negative Z coordinate because it must be positive so that uniqueness works
        data[std::stof(keySplitted[1]) * size * -1 + std::stoi(keySplitted[0])] = std::stof(value);
    } 
}




