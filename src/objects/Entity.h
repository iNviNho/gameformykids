

#ifndef ENTITY_H
#define ENTITY_H
#include "../../../src/models/Model.h"

// Represents any renderable object in the game
// (tree, grass, bridge, house, player, animal etc)
class Entity {
public:
    // TODO: Does this have to be shrd pointer?
    Entity(const std::shared_ptr<Model> &model, glm::vec3 position):
        model(model), position(position), rotation(glm::vec3(0.0f)) {}
    ~Entity() = default;

    void RotateX(float angle) { rotation.x += angle; }
    void SetRotateX(float angle) { rotation.x = angle; }
    void RotateY(float angle) { rotation.y += angle; }
    void SetRotateY(float angle) { rotation.y = angle; }
    void RotateZ(float angle) { rotation.z += angle; }
    void Move(glm::vec3 position);
    [[nodiscard]] std::shared_ptr<Model> GetModel() const { return model; }
    [[nodiscard]] const glm::vec3& GetPosition() const { return position; }
    [[nodiscard]] float GetRotationXAngle() const { return rotation.x; }
    [[nodiscard]] float GetRotationYAngle() const { return rotation.y; }
    [[nodiscard]] float GetRotationZAngle() const { return rotation.z; }
private:
    std::shared_ptr<Model> model;
    glm::vec3 position;
    glm::vec3 rotation;

    void updateRotation(glm::vec3 dir);
};


#endif //ENTITY_H
