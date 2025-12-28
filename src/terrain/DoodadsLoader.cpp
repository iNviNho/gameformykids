#include "DoodadsLoader.h"

#include "data_dir.h"
#include "EntitiesHolder.h"
#include "../storage/LocalStorage.h"
#include "../utils/Log.h"

void DoodadsLoader::LoadDoodads(
    ModelsHolder& modelsHolder,
    LocalStorage& storage,
    EntitiesHolder& entitiesHolder)
{
    Log::logInfo("Loading doodads");
    auto lines = storage.GetLines();
    for (const std::string&line : lines) {
        // first part is name
        // second part is unique id (we can ignore it)
        // rest is entity data
        auto parts = split(line, ',');
        Entity entity{
            modelsHolder.GetModel(parts[0]),
            glm::vec3(
                std::stof(parts[2]), std::stof(parts[3]), std::stof(parts[4])
            )};

        entity.SetRotateX(std::stof(parts[5]));
        entity.SetRotateY(std::stof(parts[6]));
        entity.SetRotateZ(std::stof(parts[7]));

        entity.SetScale(std::stof(parts[8]));
        entitiesHolder.AddEntity(entity);
    }
    Log::logInfo("Doodads loaded");
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