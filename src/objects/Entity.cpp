

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
    // since we know the length of adjacent and opposite sides of the triangle
    // we can use inverse of tan (arctan) to get the angle
    double angle = atan2(dir.x, dir.z) * (180.0 / M_PI);
    // we don't want to work with negative angles
    if (angle < 0) {
        angle += 360.0;
    }
    SetRotateY(angle);
}
