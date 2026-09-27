#ifndef TERRAIN_HEIGHT
#define TERRAIN_HEIGHT

#include <unordered_map>
#include <data_dir.h>
#include "../storage/BinaryLocalStorage.h"

using path = std::filesystem::path;  

class TerrainHeight {
public: 
    TerrainHeight(int size)
        : size(size), data(size * size), storage(data_dir() /= path("map/terrain_height.txt")) {
        loadDataFromStorage();
    }; 
    void SetMultiple(const std::vector<Coordinate>& coordinates);
    float Get(int x, int z) const;
private:
    std::unordered_map<int, float> data;
    BinaryLocalStorage storage;
    int size;

    void loadDataFromStorage();
};


#endif
