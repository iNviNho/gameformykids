

#include "Player.h"

constexpr float cameraDistance = 7.0f;
constexpr float cameraHeight = 3.0f;

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

    // Set the new y position based on the camera offset
    // TODO: This is a temporary solution to keep the camera above the terrain
    // Implement based on user y position & gravity
    newPosition.y = cameraHeight;

    // Update the camera's position
    camera.UpdatePosition(newPosition);
}

void Player::UpdateCameraPitch() {
    // TODO: It ignores players height
    float hypotenus = sqrt(
        pow(cameraDistance, 2) +
        pow(cameraHeight, 2)
    );
    float cosine = cameraDistance / hypotenus;
    float angleInRadians = std::acos(cosine);
    float angleInDegrees = angleInRadians * (180.0 / M_PI);
    camera.UpdatePitch(angleInDegrees * -1.0f);
}

void Player::UpdateCameraYaw() {
    camera.UpdateYaw(-90.0f - (GetRotationYAngle() - 180.0f));
}

void Player::Move(glm::vec3 pos) {
    // update y position based on terrain height
    pos.y = terrain.GetHeightOfTerrain(pos.x, pos.z) - GetPosition().y;
    // we first move entity
    Entity::Move(pos);
    // then we offset camera
    updateCameraPosition();
    UpdateCameraPitch();
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
