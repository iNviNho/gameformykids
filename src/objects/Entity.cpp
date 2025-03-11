

#include "Entity.h"

Entity::Entity(Model model, glm::vec3 position, glm::mat4 modelMatrix)
    : model(std::move(model)), position(position), modelMatrix(modelMatrix) {
}