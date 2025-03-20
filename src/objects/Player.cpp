

#include "Player.h"

// TODO: x, z must be the same for some reason
// This must be rewritten so that camera offset is defined by
// - distance from palyer
// - height of camera
const glm::vec3 Player::CAMERA_OFFSET = glm::vec3(7.0f, 3.0f, 7.0f);

// Ref: /progressScreenshots/14calculateCameraPosition.png
void Player::updateCameraPosition() {
    glm::vec3 newPosition = glm::vec3(0.0f);

    // Get the player's rotation angle in radians
    double radians = GetRotationYAngle() * (M_PI / 180.0);

    // Calculate the new x position based on the player's rotation
    // In right triangle sin(angle) = opposite / hypotenuse
    // In this case the hypotenuse is the camera offset x
    // and the opposite is the x position we are trying to find
    float xMove = sin(radians) * CAMERA_OFFSET.x;
    newPosition.x = GetPosition().x - xMove;

    // Calculate the new z position based on the player's rotation
    // In right triangle cos(angle) = adjacent / hypotenuse
    // In this case the hypotenuse is the camera offset z
    // and the adjacent is the z position we are trying to find
    float zMove = cos(radians) * CAMERA_OFFSET.z;
    newPosition.z = GetPosition().z - zMove;

    // Set the new y position based on the camera offset
    // TODO: This is a temporary solution to keep the camera above the terrain
    // Implement based on user y position & gravity
    newPosition.y = CAMERA_OFFSET.y;

    // Update the camera's position
    camera.UpdatePosition(newPosition);
}

void Player::UpdateCameraPitch() {
    float hypotenus = sqrt(
        pow(CAMERA_OFFSET.x, 2) +
        pow(CAMERA_OFFSET.y, 2)
    );
    float cosine = CAMERA_OFFSET.x / hypotenus;
    float angleInRadians = std::acos(cosine);
    float angleInDegrees = angleInRadians * (180.0 / M_PI);
    camera.UpdatePitch(angleInDegrees * -1.0f);
}

void Player::UpdateCameraYaw() {
    camera.UpdateYaw(-90.0f - (GetRotationYAngle() - 180.0f));
}

void Player::Move(glm::vec3 pos) {
    // we first move entity
    Entity::Move(pos);
    // then we offset camera
    updateCameraPosition();
}

void Player::SetRotateY(float angle) {
    // we first rotate entity
    Entity::SetRotateY(angle);
    // then we update camera yaw
    UpdateCameraYaw();
}

void Player::SetRotateX(float angle) {
    // we first rotate entity
    Entity::SetRotateX(angle);
    // then we update camera pitch
    UpdateCameraPitch();
}
