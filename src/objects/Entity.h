

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
    void MoveBy(const glm::vec3& moveVector);
    void MoveTo(const glm::vec3& position);
    const Model& GetModel() const { return *model; }
    const glm::vec3& GetPosition() const { return position; }
    float GetRotationXAngle() const { return rotation.x; }
    float GetRotationYAngle() const { return rotation.y; }
    float GetRotationZAngle() const { return rotation.z; }
private:
    std::shared_ptr<Model> model;
    glm::vec3 position;
    glm::vec3 rotation;

    void updateRotation(const glm::vec3& direction);
};


#endif //ENTITY_H
