#ifndef MYGAME_MODELSHOLDER_H
#define MYGAME_MODELSHOLDER_H
#include "Model.h"


class ModelsHolder {
public:
    ModelsHolder() {}
    void LoadModels();
    std::shared_ptr<Model>& GetModel(const std::string& modelName);
    std::vector<std::string> GetModelNames() const;
private:
    std::unordered_map<std::string, std::shared_ptr<Model>> loadedModels;
};


#endif //MYGAME_MODELSHOLDER_H