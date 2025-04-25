

#include "Player.h"

constexpr float cameraDistance = 12.0f;
constexpr float cameraHeight = 6.5f;

// Ref: /progressScreenshots/14calculateCameraPosition.png
void Player::updateCameraPosition() {
    auto newPosition = glm::vec3(0.0f);

    // Get the player's rotation angle in radians
    double radians = GetRotationYAngle() * (M_PI / 180.0);

    // Calculate the new x position based on the player's rotation
    // In right triangle sin(angle) = opposite / hypotenuse
    // In this case the hypotenuse is the camera offset x
    // and the opposite is the x position we are trying to find
    float xMove = sin(radians) * cameraDistance;
    newPosition.x = GetPosition().x - xMove;

    // Calculate the new z position based on the player's rotation
    // In right triangle cos(angle) = adjacent / hypotenuse
    // In this case the hypotenuse is the camera offset z
    // and the adjacent is the z position we are trying to find
    float zMove = cos(radians) * cameraDistance;
    newPosition.z = GetPosition().z - zMove;

    newPosition.y = cameraHeight + GetPosition().y;

    // Update the camera's position
    camera.UpdatePosition(newPosition);
}

void Player::updateCameraPitch() {
    float hypotenus = sqrt(
        pow(cameraDistance, 2) +
        pow(cameraHeight, 2)
    );
    float cosine = cameraDistance / hypotenus;
    float angleInRadians = std::acos(cosine);
    float angleInDegrees = angleInRadians * (180.0 / M_PI);
    camera.UpdatePitch(angleInDegrees * -1.0f);
}

void Player::updateCameraYaw() {
    double yaw = -GetRotationYAngle() + 90.0f;
    if (yaw < 0) {
        yaw += 360.0;
    }
    camera.UpdateYaw(yaw);
}

void Player::Move(glm::vec3 pos) {
    // update y position based on terrain height
    pos.y = terrain.GetHeightOfTerrain(GetPosition().x, GetPosition().z) - GetPosition().y;
    // we first move entity
    Entity::Move(pos);
    // then we offset camera
    updateCameraPosition();
    updateCameraPitch();
    updateCameraYaw();
}

void Player::Move(glm::vec3 dir, float distance) {
    float dirLengthSquared = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    if (dirLengthSquared > 0.0001f) { // Avoid division by zero or moving in zero direction
        glm::vec3 normalizedDir = glm::normalize(dir);

        Move(normalizedDir * distance);
    }
}
