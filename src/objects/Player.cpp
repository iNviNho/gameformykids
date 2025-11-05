

#include "Player.h"
#include <cmath>

constexpr float cameraDistance = 12.0f;
constexpr float cameraHeight = 6.5f;

// Ref: /progressScreenshots/14calculateCameraPosition.png
void Player::UpdateCameraPosition(bool animated) {
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
    camera.UpdatePosition(newPosition, animated);
}

void Player::updateCameraPitch() {
    float angleInRadians = std::atan(cameraHeight /  cameraDistance);
    float angleInDegrees = angleInRadians * (180.0 / M_PI);
    camera.UpdatePitch(angleInDegrees);
}

void Player::UpdateCameraYaw(bool animated) {
    double yaw = 180.0 - GetRotationYAngle();
    if (yaw < 0) {
        yaw += 360.0;
    }
    camera.UpdateYaw(yaw, animated);
}

/**
 * Moves player in given direction by given distance.
 */
void Player::MoveIn(const glm::vec3& normalizedDir, float distance) {
    MoveBy(normalizedDir * distance);
}
