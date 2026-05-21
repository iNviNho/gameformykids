#ifndef TERRAIN_HEIGHT
#define TERRAIN_HEIGHT


#include <stdexcept>
#include <string>
#include <unordered_map>
#include "../storage/LocalStorage.h"
#include <data_dir.h>
#include "../tracing/Tracer.h"

using path = std::filesystem::path;  

class TerrainHeight {
    public: 
        TerrainHeight(int size)
            : size(size), data(size * size), storage(data_dir() /= path("resources/map/terrain_height.txt")) {
            loadDataFromStorage();
        }; 
        void set(int x, int z, float height) {
            if (x < 0 || z > 0) {
                throw std::runtime_error("Unable to store height if one of the x or z coordinates is negative"); 
            }
            // we modify z to be positive so that the uniqueness of this coordinate works
            data[z * size * -1 + x] = height;
            std::string data = std::to_string(x) + "," + std::to_string(height) + "," + std::to_string(z);
            storage.insert(std::to_string(x) + ',' + std::to_string(z), data);
        }
        void setMultiple(const std::vector<glm::vec3>& coordinates) {
            TRACE_ME();
            std::unordered_map<std::string, std::string> dataToWrite;
            for (const auto& value: coordinates) {
                dataToWrite[std::to_string(static_cast<int>(value.x)) +
                    ',' +
                    std::to_string(static_cast<int>(value.z))] = std::to_string(value.y); 
            }

            storage.insertMultiple(dataToWrite);
        }
        float get(int x, int z) const {
            auto it = data.find(z * size * -1 + x);
            return it == data.end() ? 0.0f: it->second;
        };
        void loadDataFromStorage(); 
    private:
        std::unordered_map<int, float> data;
        LocalStorage storage;
        int size;
};


#endif
