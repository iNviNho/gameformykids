#ifndef MYGAME_BONEINFORMATION_H
#define MYGAME_BONEINFORMATION_H

#include "../shaders/shader.h"

struct BoneInformation{
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransformation;

    BoneInformation(const glm::mat4& offsetMatrix)
        : offsetMatrix(offsetMatrix), finalTransformation() {
    }
};

#endif //MYGAME_BONEINFORMATION_H