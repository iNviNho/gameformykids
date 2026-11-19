#include "ModelsHolder.h"

#include "AnimatedModel.h"
#include "data_dir.h"
#include "StaticModel.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

void ModelsHolder::LoadModels() {
    // Load the export in place so changes made by wow.export are picked up
    // without copying the assets into the application's resources directory.
    std::filesystem::path modelsDirectory;
    if (const char* home = std::getenv("HOME"); home != nullptr && *home != '\0') {
        modelsDirectory = std::filesystem::path(home) / "wow.export";
    } else {
        modelsDirectory = data_dir() / "objects" / "wow.export";
    }

    if (!std::filesystem::exists(modelsDirectory)) {
        std::cerr << "Model directory does not exist: " << modelsDirectory << '\n';
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(
             modelsDirectory, std::filesystem::directory_options::skip_permission_denied)) {
        if (!entry.is_regular_file()) continue;

        auto extension = entry.path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        const bool isGltf = extension == ".gltf";
        const bool isObj = extension == ".obj";
        if (!isGltf && !isObj) continue;

        // Prefer a short key such as "cow". Use the relative path if another
        // folder contains a model with the same filename.
        auto key = entry.path().stem().string();
        if (loadedModels.find(key) != loadedModels.end()) {
            key = std::filesystem::relative(entry.path(), modelsDirectory)
                      .replace_extension().generic_string();
        }
        std::shared_ptr<AbstractModel> model;
        if (isGltf) {
            model = std::make_shared<AnimatedModel>(entry.path());
        } else {
            model = std::make_shared<StaticModel>(entry.path());
        }
        if (model->GetMeshes().empty()) {
            std::cerr << "Skipping glTF that did not load: " << entry.path() << '\n';
            continue;
        }
        loadedModels.emplace(std::move(key), std::move(model));
    }
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
