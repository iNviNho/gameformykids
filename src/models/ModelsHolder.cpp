#include "ModelsHolder.h"

#include "data_dir.h"

void ModelsHolder::LoadModels() {
    // GRASS
    std::shared_ptr<Model> grass = std::make_shared<Model>(
        data_dir() /= std::filesystem::path("resources/objects/grass6/grass.obj")
    );
    loadedModels.emplace("grass", grass);

    // PINE TREE
    std::shared_ptr<Model> pineTree = std::make_shared<Model>(
        data_dir() /= std::filesystem::path("resources/objects/pinetree/pinetree.obj")
    );
    loadedModels.emplace("pinetree", pineTree);
}

std::shared_ptr<Model>& ModelsHolder::GetModel(const std::string &modelName) {
    auto it = loadedModels.find(modelName);
    if (it != loadedModels.end()) {
        return it->second;
    }

    throw std::runtime_error("No model with name '" + modelName + "'");
}

std::vector<std::string> ModelsHolder::GetModelNames() const {
    std::vector<std::string> names;
    for (const auto& pair : loadedModels) {
        names.push_back(pair.first);
    }
    return names;
}
