

#include "Entity.h"
#include "glm/geometric.hpp"
#include "../utils/Log.h"
#include <string>

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

/**
 * Moves entity to a given position, and updates its orientation to face the given direction.
 */
void Entity::MoveTo(const glm::vec3& position, const glm::vec3& direction) {
    float beforeMoveZ = GetPosition().z;
    MoveTo(position);
    float afterMoveZ = GetPosition().z;

    const bool isMovingForward = beforeMoveZ > afterMoveZ;
    const float movingForwardModifier = isMovingForward ? 1.0f : -1.0f;

    // we have an offset based on a size of an entity but for now we use small one
    float offset = 0.25f;
    // TODO: Move to separate method
    auto positionInFront = position + (glm::normalize(direction) * movingForwardModifier) * offset;
    pointOnTerrainInFront = glm::vec3{
        positionInFront.x,
        terrain->GetHeight(positionInFront.x, positionInFront.z),
        positionInFront.z
    };

    auto positionInBack = position - (glm::normalize(direction) * movingForwardModifier) * offset;
    pointOnTerrainInBack = glm::vec3{
        positionInBack.x,
        terrain->GetHeight(positionInBack.x, positionInBack.z),
        positionInBack.z

    };

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 forward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
    
    glm::vec3 left  = glm::normalize(glm::cross(up, forward));
    glm::vec3 right = -left;

    glm::vec3 positionInLeft  = position + left * offset;
    glm::vec3 positionInRight = position + right * offset;
   
    pointOnTerrainInLeft = glm::vec3{
        positionInLeft.x,
        terrain->GetHeight(positionInLeft.x, positionInLeft.z),
        positionInLeft.z
    };
    
    pointOnTerrainInRight = glm::vec3{
        positionInRight.x,
        terrain->GetHeight(positionInRight.x, positionInRight.z),
        positionInRight.z
    };

    updateRotation(direction);
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

    glm::vec2 frontXZ(pointOnTerrainInFront.x, pointOnTerrainInFront.z);
    glm::vec2 backXZ(pointOnTerrainInBack.x, pointOnTerrainInBack.z);
    
    // here we compute height difference
    float deltaY = pointOnTerrainInFront.y - pointOnTerrainInBack.y;
    // and we get length of one side 
    float distance = glm::length(frontXZ - backXZ);
    
    float angleRadians = std::atan(deltaY / distance);
    float angleDegrees = glm::degrees(angleRadians);
    
    SetRotateX(angleDegrees);

    frontXZ = glm::vec2(pointOnTerrainInLeft.x, pointOnTerrainInLeft.z);
    backXZ = glm::vec2(pointOnTerrainInRight.x, pointOnTerrainInRight.z);
    
    // here we compute height difference
    deltaY = pointOnTerrainInLeft.y - pointOnTerrainInRight.y;
    // and we get length of one side 
    distance = glm::length(frontXZ - backXZ);
    
    angleRadians = std::atan(deltaY / distance);
    angleDegrees = glm::degrees(angleRadians);
    
    SetRotateZ(angleDegrees);
}

std::string Entity::toString() {
    std::string data;

    // first we dump position
    data += std::to_string(position.x) + "," +
            std::to_string(position.y) + "," +
            std::to_string(position.z) + ",";
    // then rotation
    data += std::to_string(rotation.x) + "," +
            std::to_string(rotation.y) + "," +
            std::to_string(rotation.z) + ",";
    // then scale
    data += std::to_string(scale);

    return data;
}
