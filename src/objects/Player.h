

#ifndef PLAYER_H
#define PLAYER_H
#include <memory>
#include <variant>
#include <glm/vec3.hpp>
#include "Entity.h"
#include "../camera/camera.h"
#include "../terrain/Terrain.h"

class Player : public Entity {
public:

    struct Stationary { };

    struct Moving {
        float speed;
        glm::vec3 dir;
    };

    struct JumpingOnPath {
        glm::vec3 terrainPos;
        float terrainSpeed;
        float velocity;
    };

    using State = std::variant<Stationary, Moving, JumpingOnPath>;

private:
    Camera& camera;
    Terrain& terrain;

    /**
     * The previous state of the player. Needed for temporary states, such as jumping, that are
     * intended to return to the previous state upon completion.
     */
    State prevState;

    /**
     * The current state of the player.
     */
    State state;

    /**
     * Set Camera::Pitch such that the camera is pointing at the player.
     *
     * As described in Camera::Pitch, we need to compute the rotation around the x-axis that brings
     * the player into view, which is a rotation that transforms the yaw-rotated world
     * frame into the camera frame.
     *
     * Since the camera is positioned above the player (see Player::updateCameraPosition(bool)),
     * the player's (x,y,z) position in the yaw-rotated world frame is (0, negative camera height, camera distance).
     * The player's orientation around the x-axis is then <tt> atan(-camera height / camera distance) </tt>.
     *
     * The camera is oriented toward the negative z-axis in the camera frame, so its orientation around
     * the x-axis is 0 degrees (0 radians).
     *
     * In order for the camera to point directly at the player from its position,
     * the player's orientation around the x-axis in the camera frame must be the same as the camera's
     * orientation around the x-axis.
     *
     * The rotation around the x-axis that transforms the player's orientation around the x-axis
     * in the yaw-rotated world frame to its orientation in the camera frame is then the angle we need to add
     * to the player's orientation to make it equal to the camera's orientation.
     * This amount is just the difference between 0 degrees (0 radians) and the <tt> atan(-camera height / camera distance) </tt>.
     * Adding this amount to the player's orientation makes it the same as the camera's orientation.
     *
     * Note that <tt> 0 - atan(-camera height / camera distance) = atan(camera height / camera distance) </tt>.
     */
    void updateCameraPitch();

    void UpdateCameraPosition(bool animated = true);

    /**
     * Set Camera::Yaw such that the camera is pointing at the player.
     *
     * As described in Camera::Yaw, we need to compute the rotation around the y-axis that brings
     * the player into view, which is a rotation that transforms the world frame
     * into the camera frame.
     *
     * The player's orientation around the y-axis in the world frame is given by Player::GetRotationYAngle(),
     * which itself is determined from the player's movement direction.
     *
     * The camera is oriented toward the negative z-axis in the camera frame, so its orientation around
     * the y-axis is 180 degrees (π radians).
     *
     * In order for the camera to point directly at the player from its position behind the player (see Player::updateCameraPosition(bool)),
     * the player's orientation around the y-axis in the camera frame must be the same as the camera's
     * orientation around the y-axis.
     *
     * The rotation around the y-axis that transforms the player's orientation around the y-axis
     * in the world frame to its orientation in the camera frame is then the angle we need to add
     * to the player's orientation to make it equal to the camera's orientation.
     * This amount is just the difference between 180 degrees (π radians) and the player's orientation.
     * Adding this amount to the player's orientation makes it the same as the camera's orientation.
     *
     * @param[in] animated
     * @parblock
     * If @c true, the camera will smoothly transition to the new yaw angle.
     *
     * If @c false, the camera's yaw angle will be set immediately to the new angle.
     * @endparblock
     */
    void UpdateCameraYaw(bool animated = true);

public:
    Player(Camera &camera, Terrain &terrain, const std::shared_ptr<Model> &model, glm::vec3 position):
        Entity(model, position), terrain(terrain), camera(camera), prevState{ Stationary{} }, state{ Stationary{} } {
    }

    constexpr const State& getState() const noexcept { return state; }

    constexpr State& getState() noexcept { return state; }

    constexpr void setState(const Player::State& newState) noexcept
    {
        if (state.index() != newState.index())
            prevState = state;
        state = newState;
    }

    constexpr const State& getPrevState() const noexcept { return prevState; }

    void MoveIn(const glm::vec3& dir, float distance);

    /**
     * Updates camera position, pitch and yaw to match the new player position.
     */
    void UpdateCameraPose(bool animated = true)
    {
        UpdateCameraPosition(animated);
        updateCameraPitch();
        UpdateCameraYaw(animated);
    }

    Camera& GetCamera() const { return camera;}
};

#endif //PLAYER_H
