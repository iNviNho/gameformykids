

#ifndef MODELGENERATOR_H
#define MODELGENERATOR_H

#include "Model.h"


class ModelGenerator {
public:
    static Model* generateCube(char *texturePath);
    static Model* generateGrass(char *texturePath);
};


#endif //MODELGENERATOR_H
