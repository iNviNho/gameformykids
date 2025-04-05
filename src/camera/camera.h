#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float TargetYaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    ) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        TargetPosition = position;
        WorldUp = up;
        Yaw = yaw;
        TargetYaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
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
        Yaw += yawDiff * deltaTime;

        // TODO: Only update once position or yaw diff is so small it doesn't make sense to continue
        updateCameraVectors();
    }

    void UpdatePosition(glm::vec3 position) {
        if (position == TargetPosition) {
            return;
        }
        TargetPosition = position;
    }

    void UpdateYaw(float yaw) {
        if (yaw == TargetYaw) {
            return;
        }
        TargetYaw = yaw;
    }

    void UpdatePitch(float pitch) {
        if (Pitch == pitch) {
            return;
        }
        Pitch = pitch;
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif