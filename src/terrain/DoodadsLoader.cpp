#include "DoodadsLoader.h"

#include "data_dir.h"
#include "EntitiesHolder.h"
#include "../storage/LocalStorage.h"
#include "../utils/Log.h"

void DoodadsLoader::LoadDoodads(LocalStorage& storage, EntitiesHolder& entitiesHolder) {

    auto lines = storage.GetLines();
    for (const std::string&line : lines) {
        // log first
        Log::logInfo(line);

        // first part is name
        // second part is unique id (we can ignore it)
        // rest is entity data
        auto parts = split(line, ',');
        if (parts.size() < 8) {
            continue; // invalid line
        }

        // add object
        std::shared_ptr<Model> grass = std::make_shared<Model>(
            data_dir() /= std::filesystem::path("resources/objects/grass6/grass.obj")
        );

        Entity entity{grass, glm::vec3(
            std::stof(parts[2]), std::stof(parts[3]), std::stof(parts[4]))};
        entitiesHolder.AddEntity(entity);
    }
}

// Split a string by delimiter
std::vector<std::string> DoodadsLoader::split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(str);
    std::string part;

    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}