

#ifndef ENTITY_H
#define ENTITY_H
#include "../../../src/models/Model.h"


class Entity {
public:
    Entity(Model model, glm::vec3 position, glm::mat4 modelMatrix);
    ~Entity() = default;
    [[nodiscard]] const Model& GetModel() const { return model; }
    [[nodiscard]] const glm::mat4& GetModelMatrix() const { return modelMatrix; }
    [[nodiscard]] const glm::vec3& GetPosition() const { return position; }
private:
    Model model;
    glm::vec3 position;
    glm::mat4 modelMatrix;
};


#endif //ENTITY_H
