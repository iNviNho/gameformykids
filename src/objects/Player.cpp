

#include "Player.h"

#include "../utils/Log.h"
#include "GLFW/glfw3.h"

constexpr float cameraDistance = 12.0f;
constexpr float cameraHeight = 6.5f;

// for higher jumps, increase
constexpr float JUMP_FORCE = 5.0f;
constexpr float GRAVITY = -9.8f;
float currentJumpHeight = 0.0f;
float velocityY = 0.0f;

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

// TODO: Refactor move methods and streamline usage
void Player::Move(glm::vec3 pos) {
    // update y position based on terrain height
    pos.y = terrain.GetHeightOfTerrain(GetPosition().x, GetPosition().z) - GetPosition().y;
    // we first move entity
    Entity::Move(pos);
    // then we offset camera
    UpdateCameraPosition();
    updateCameraPitch();
    UpdateCameraYaw();
}

void Player::Move(glm::vec3 dir, float distance) {
    float dirLengthSquared = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    if (dirLengthSquared > 0.0001f) { // Avoid division by zero or moving in zero direction
        glm::vec3 normalizedDir = glm::normalize(dir);

        // Clamp distance to prevent large movements due to high deltaTime
        float maxDistance = 1.0f; // Adjust as needed
        distance = glm::min(distance, maxDistance);

        Move(normalizedDir * distance);
    }
}

void Player::Jump() {
    if (!jumped) {
        Log::logInfo("Player jumped");
        jumped = true;
        jumpedAt = glfwGetTime();
        velocityY = JUMP_FORCE;
    }
}

void Player::handleJump(float deltaTime) {
    if (jumped) {
        // get current position
        glm::vec3 newPos = GetPosition();

        // apply gravity
        velocityY += GRAVITY * deltaTime;
        float heightToAdd = velocityY * deltaTime;
        newPos.y += heightToAdd + currentJumpHeight;
        // we need to keep track of currentJumpHeight
        // because PathPlayerMover changes("resets") y position with
        // every new frame to match the terrain
        currentJumpHeight += heightToAdd;

        if (newPos.y <= GetPosition().y) {
            jumped = false;
            jumpedAt = glfwGetTime();
            currentJumpHeight = 0.0f;
        } else {
            SetPosition(newPos);
        }
    }
}