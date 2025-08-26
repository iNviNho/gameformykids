#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../utils/Log.h"

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
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 TargetPosition;

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
     * Target rotation of the world around the Y axis, in degrees.
     */
    float TargetYaw;

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
    ) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        TargetPosition = position;
        Yaw = yaw;
        TargetYaw = yaw;
        Pitch = pitch;
        updateViewMatrix();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const
    {
        return viewMatrix;
    }


    glm::vec3 GetTargetPosition() const {
        return TargetPosition;
    }

    float GetTargetYaw() const {
        return TargetYaw;
    }

    // Not needed for this game, camera moves based on player position
    // // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    // void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    // {
    //     float velocity = MovementSpeed * deltaTime;
    //     if (direction == FORWARD)
    //         Position += Front * velocity;
    //     if (direction == BACKWARD)
    //         Position -= Front * velocity;
    //     if (direction == LEFT)
    //         Position -= Right * velocity;
    //     if (direction == RIGHT)
    //         Position += Right * velocity;
    //     if (direction == UP)
    //         Position += Up * velocity;
    //     if (direction == DOWN)
    //         Position -= Up * velocity;
    // }

    void tick(float deltaTime) {
        // we will move the camera towards the target position with a small velocity
        glm::vec3 positionDiff = TargetPosition - Position;
        Position += positionDiff * deltaTime;

        float yawDiff = TargetYaw - Yaw;
        if (yawDiff > 180.0)
            yawDiff -= 360.0;
        else if (yawDiff < -180.0)
            yawDiff += 360.0;

        Yaw += yawDiff * deltaTime;

        // TODO: Only update once position or yaw diff is so small it doesn't make sense to continue
        updateViewMatrix();
    }

    void UpdatePosition(glm::vec3 position, bool animated) {
        TargetPosition = position;
        if (!animated) {
            Position = position;
        }
    }

    void UpdateYaw(float yaw, bool animated = true) {
        TargetYaw = yaw;
        if (!animated) {
            Yaw = yaw;
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

private:

    /**
     * Calculates the view matrix from the Camera's (updated) Euler Angles
     *
     * The matrix is derived as follows:
     *
     * const glm::mat3 r_yaw = glm::mat3{ cos_yaw, 0, -sin_yaw,
     *                                          0, 1,        0,
     *                                    sin_yaw, 0,  cos_yaw };
     * const glm::mat3 r_pitch = glm::mat3{ 1,          0,         0,
     *                                      0,  cos_pitch, sin_pitch,
     *                                      0, -sin_pitch, cos_pitch };
     * const glm::mat4 r{ r_pitch * r_yaw };
     *
     * viewMatrix = r * glm::mat4{ glm::vec4{1,0,0,0},
     *                             glm::vec4{0,1,0,0},
     *                             glm::vec4{0,0,1,0},
     *                             glm::vec4{-Position.x, -Position.y, -Position.z, 1} } ;
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