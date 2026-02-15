

#ifndef MODELGENERATOR_H
#define MODELGENERATOR_H

#include <filesystem>
#include "AbstractModel.h"


class ModelGenerator {
public:
    static std::shared_ptr<AbstractModel> generateCube(const std::filesystem::path& texturePath);
    static std::shared_ptr<AbstractModel> generateGrass(const std::filesystem::path& texturePath);
};


#endif //MODELGENERATOR_H
