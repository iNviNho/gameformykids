

#include "Entity.h"

void Entity::Move(glm::vec3 position) {
    glm::vec3 newPosition = this->position + position;
    if (newPosition.x < 0.0f) {
        newPosition.x = 0.0f;
    }
    this->position = newPosition;
}
