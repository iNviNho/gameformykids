

#include "Entity.h"

/**
 * Moves entity by a given position vector.
 */
void Entity::MoveBy(const glm::vec3& moveVector) {
    glm::vec3 newPosition = this->position + moveVector;
    if (newPosition.x < 0.0f) {
        newPosition.x = 0.0f;
    }
    this->position = newPosition;

    // update entity rotation after movement
    updateRotation(moveVector);
}

/**
 * Moves entity to a given position.
 */
void Entity::MoveTo(const glm::vec3& position) {
    this->position = position;
}

void Entity::updateRotation(const glm::vec3& direction) {
    // since we know the length of adjacent and opposite sides of the triangle
    // we can use inverse of tan (arctan) to get the angle
    float angle = atan2(direction.x, direction.z) * (180.0 / M_PI);
    // we don't want to work with negative angles
    if (angle < 0) {
        angle += 360.0;
    }
    // TODO: In the future update rotation also in x and z axis based on the position vs terrain
    SetRotateY(angle);
}
