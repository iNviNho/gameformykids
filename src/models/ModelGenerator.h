

#ifndef MODELGENERATOR_H
#define MODELGENERATOR_H

#include <filesystem>
#include "Model.h"


class ModelGenerator {
public:
    static std::shared_ptr<Model> generateCube(const std::filesystem::path& texturePath);
    static std::shared_ptr<Model> generateGrass(const std::filesystem::path& texturePath);
};


#endif //MODELGENERATOR_H
