#ifndef MYGAME_MODELSHOLDER_H
#define MYGAME_MODELSHOLDER_H
#include "AbstractModel.h"


class ModelsHolder {
public:
    ModelsHolder() = default;
    void LoadModels();
    std::shared_ptr<AbstractModel>& GetModel(const std::string& modelName);
    std::vector<std::string> GetModelNames() const;
private:
    std::unordered_map<std::string, std::shared_ptr<AbstractModel>> loadedModels;
};


#endif //MYGAME_MODELSHOLDER_H