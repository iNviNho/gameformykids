

#ifndef MODELGENERATOR_H
#define MODELGENERATOR_H

#include "Model.h"


class ModelGenerator {
public:
    static Model generateCube(glm::vec3 position, char *texturePath);
};



#endif //MODELGENERATOR_H
