#include "ModelsHolder.h"

#include "AnimatedModel.h"
#include "data_dir.h"
#include "StaticModel.h"

void ModelsHolder::LoadModels() {
    // GRASS
    std::shared_ptr<AbstractModel> grass = std::make_shared<StaticModel>(
        data_dir() /= std::filesystem::path("resources/objects/grass6/grass.obj")
    );
    loadedModels.emplace("grass", grass);

    // PINE TREE
    std::shared_ptr<AbstractModel> pineTree = std::make_shared<StaticModel>(
        data_dir() /= std::filesystem::path("resources/objects/pinetree/pinetree.obj")
    );
    loadedModels.emplace("pinetree", pineTree);

    // BOB WITH LAMP
    std::shared_ptr<AbstractModel> bobWithLamp = std::make_shared<AnimatedModel>(
        data_dir() /= std::filesystem::path("resources/objects/animals/bob/boblampclean.md5mesh"))
    ;
    loadedModels.emplace("bobwithlamp", bobWithLamp);
}

std::shared_ptr<AbstractModel>& ModelsHolder::GetModel(const std::string &modelName) {
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
