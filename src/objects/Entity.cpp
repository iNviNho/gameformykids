

#include "Entity.h"

void Entity::Move(glm::vec3 position) {
    glm::vec3 newPosition = this->position + position;
    if (newPosition.x < 0.0f) {
        newPosition.x = 0.0f;
    }
    this->position = newPosition;

    // update entity rotation y based on movement
    updateRotation(position);
}

void Entity::updateRotation(glm::vec3 dir) {
    SetRotateY(atan2(dir.x, dir.z) * (180.0 / M_PI));
}
