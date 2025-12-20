#ifndef CAMERA_H
#define CAMERA_H

#include <chrono>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "../utils/Log.h"
#include "../utils/lerp.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

// Default camera values
const float YAW         =  0.0f;
const float PITCH       =  0.0f;
const float SPEED       =  5.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
    /**
     * Duration of camera pose update animation.
     */
    constexpr static std::chrono::high_resolution_clock::duration ANIMATION_DURATION{ std::chrono::milliseconds{300} };

    /**
     * Elapsed time since the start of animation.
     *
     * Never more than Camera::ANIMATION_DURATION;
     */
    std::chrono::high_resolution_clock::duration elapsed;

    /**
     * Position of the camera at the start of animation.
     */
    glm::vec3 StartPosition;

    /**
     * Position of the camera at the end of animation.
     */
    glm::vec3 TargetPosition;

    /**
     * Rotation of the world around the Y axis, in degrees, at the start of animation.
     */
    float StartYaw;

    /**
     * Rotation of the world around the Y axis, in degrees, at the end of animation.
     */
    float TargetYaw;

public:
    // camera Attributes
    glm::vec3 Position;

    /**
     * Current transform from the world coordinates to camera coordinates.
     *
     * Defined by the camera's Position, and Yaw, Pitch angles that
     * describe the needed rotation of the world to align with the camera.
     *
     * The rotation is defined from Yaw, Pitch by first applying rotation
     * around the y-axis (Yaw), and then applying rotation around the x-axis
     * (Pitch).
     */
    glm::mat4 viewMatrix;

    /**
     * Current rotation around the y-axis that brings the world-frame into view, in degrees.
     */
    float Yaw;

    /**
     * Current rotation around the x-axis that brings the Yaw-rotated world into view, in degrees.
     */
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    ) : elapsed{ ANIMATION_DURATION }, StartPosition(position), TargetPosition(position),
        StartYaw(yaw), TargetYaw(yaw),
        Position(position),
        Yaw(yaw),
        Pitch(pitch),
        MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        updateViewMatrix();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    const glm::mat4& GetViewMatrix() const
    {
        return viewMatrix;
    }


    glm::vec3 GetTargetPosition() const {
        return TargetPosition;
    }

    float GetTargetYaw() const {
        return TargetYaw;
    }

    glm::vec3 GetFrontVector() const {
        glm::mat4 view = GetViewMatrix();
        glm::mat3 rotation = glm::mat3(glm::transpose(view));
        return -rotation[2]; // Forward (negative Z in view space)
    }

    // processes input received from any keyboard-like input system
    // accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    // works only in game edit mode
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        glm::mat4 view = GetViewMatrix();
        glm::mat3 rotation = glm::mat3(glm::transpose(view)); // Inverse rotation

        glm::vec3 front = -rotation[2]; // Forward (negative Z in view space)
        glm::vec3 right = rotation[0];  // Right (X in view space)
        glm::vec3 up    = rotation[1];  // Up (Y in view space)

        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            TargetPosition += front * velocity;
        if (direction == BACKWARD)
            TargetPosition -= front * velocity;
        if (direction == LEFT)
            TargetPosition -= right * velocity;
        if (direction == RIGHT)
            TargetPosition += right * velocity;
        if (direction == UP)
            TargetPosition += up * velocity;
        if (direction == DOWN)
            TargetPosition -= up * velocity;
    }

    void tick(float deltaTime) {
        if (elapsed < ANIMATION_DURATION) {
            elapsed += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(std::chrono::duration<float, std::chrono::seconds::period>(deltaTime));
            if (elapsed > ANIMATION_DURATION)
                elapsed = ANIMATION_DURATION;
        }

        const float frac = static_cast<float>(elapsed.count()) / static_cast<float>(ANIMATION_DURATION.count());

        Position = glm::vec3{ lerp(StartPosition.x, TargetPosition.x, frac),
                              lerp(StartPosition.y, TargetPosition.y, frac),
                              lerp(StartPosition.z, TargetPosition.z, frac) };

        Yaw = lerp(StartYaw, TargetYaw, frac);

        // TODO: Only update once position or yaw diff is so small it doesn't make sense to continue
        updateViewMatrix();
    }

    void UpdatePosition(glm::vec3 position, bool animated) {
        TargetPosition = position;
        if (!animated) {
            Position = position;
            StartPosition = position;
            elapsed = ANIMATION_DURATION;
        }
        else {
            StartPosition = Position;
            elapsed = std::chrono::high_resolution_clock::duration::zero();
        }
    }

    void UpdateYaw(float yaw, bool animated = true) {
        TargetYaw = yaw;
        if (!animated) {
            Yaw = yaw;
            StartYaw = yaw;
            elapsed = ANIMATION_DURATION;
        }
        else {
            const float yawDiff = TargetYaw - Yaw;
            if (yawDiff > 180.0f)
                Yaw += 360.0f;
            else if (yawDiff < -180.0f)
                Yaw -= 360.0f;

            StartYaw = Yaw;
            elapsed = std::chrono::high_resolution_clock::duration::zero();
        }
    }

    void UpdatePitch(float pitch) {
        if (Pitch == pitch) {
            return;
        }
        Pitch = pitch;
        updateViewMatrix();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 90.0f)
            Zoom = 90.0f;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        TargetYaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateViewMatrix();
    }

private:

    /**
     * Calculates the view matrix from Camera::Position, Camera::Yaw, and Camera::Pitch.
     *
     * The view matrix \f$\mathbf{M}\f$ is derived as follows:
     *
     * \f[
     * \mathbf{R}_{yaw} = \left[
     * \begin{array}{ccc}
     * \  \cos yaw & 0 & \sin yaw \ \\
     * \  0        & 1 & 0        \ \\
     * \ -\sin yaw & 0 & \cos yaw \ 
     * \end{array}
     * \right]
     * \f]
     *
     * \f[
     * \mathbf{R}_{pitch} = \left[
     * \begin{array}{ccc}
     * \ 1 & 0          &  0          \ \\
     * \ 0 & \cos pitch & -\sin pitch \ \\
     * \ 0 & \sin pitch &  \cos pitch \ 
     * \end{array}
     * \right]
     * \f]
     *
     * \f[
     * \mathbf{R} = \mathbf{R}_{pitch} * \mathbf{R}_{yaw}
     * \f]
     *
     * \f[
     * \mathbf{M} = \left[
     * \begin{array}{cc}
     * \ \mathbf{R}            & \mathbf{0} \ \\
     * \ \mathbf{0}^\mathrm{T} & 1          \ 
     * \end{array}
     * \right] * \left[
     * \begin{array}{cccc}
     * \ 1 & 0 & 0 & -Position_x \ \\
     * \ 0 & 1 & 0 & -Position_y \ \\
     * \ 0 & 0 & 1 & -Position_z \ \\
     * \ 0 & 0 & 0 &  1          \ 
     * \end{array}
     * \right]
     * \f]
     *
     */
    void updateViewMatrix()
    {
        const float cos_pitch = cos(glm::radians(Pitch));
        const float sin_pitch = sin(glm::radians(Pitch));
        const float cos_yaw = cos(glm::radians(Yaw));
        const float sin_yaw = sin(glm::radians(Yaw));

        const float sin_pitch_sin_yaw = sin_pitch * sin_yaw;
        const float cos_pitch_sin_yaw = cos_pitch * sin_yaw;
        const float sin_pitch_cos_yaw = sin_pitch * cos_yaw;
        const float cos_pitch_cos_yaw = cos_pitch * cos_yaw;

        viewMatrix[0][0] = cos_yaw;
        viewMatrix[0][1] = sin_pitch_sin_yaw;
        viewMatrix[0][2] = -cos_pitch_sin_yaw;
        viewMatrix[0][3] = 0.0f;

        viewMatrix[1][0] = 0.0f;
        viewMatrix[1][1] = cos_pitch;
        viewMatrix[1][2] = sin_pitch;
        viewMatrix[1][3] = 0.0f;

        viewMatrix[2][0] = sin_yaw;
        viewMatrix[2][1] = -sin_pitch_cos_yaw;
        viewMatrix[2][2] = cos_pitch_cos_yaw;
        viewMatrix[2][3] = 0.0f;

        viewMatrix[3][0] = -Position.x * cos_yaw - Position.z * sin_yaw;
        viewMatrix[3][1] = -Position.x * sin_pitch_sin_yaw - Position.y * cos_pitch + Position.z * sin_pitch_cos_yaw;
        viewMatrix[3][2] = Position.x * cos_pitch_sin_yaw - Position.y * sin_pitch - Position.z * cos_pitch_cos_yaw;
        viewMatrix[3][3] = 1.0f;
    }
};
#endif