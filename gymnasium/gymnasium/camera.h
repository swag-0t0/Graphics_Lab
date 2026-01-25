#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float Roll;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    bool orbitMode;
    glm::vec3 orbitCenter;
    float orbitRadius;
    float orbitAngle;

    Camera(glm::vec3 position = glm::vec3(0.0f, 5.0f, 20.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Roll(0.0f),
        orbitMode(false), orbitCenter(glm::vec3(0.0f, 0.0f, 0.0f)),
        orbitRadius(20.0f), orbitAngle(0.0f)
    {
            Position = position;
            Position = glm::vec3(35.0f, 8.0f, 0.0f);
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        if (orbitMode) {
            return glm::lookAt(Position, orbitCenter, Up);
        }
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void RotatePitch(float angle) {
        Pitch += angle;
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
        updateCameraVectors();
    }

    void RotateYaw(float angle) {
        Yaw += angle;
        updateCameraVectors();
    }

    void RotateRoll(float angle) {
        Roll += angle;
        updateCameraVectors();
    }

    void ToggleOrbitMode() {
        orbitMode = !orbitMode;
        if (orbitMode) {
            orbitCenter = Position + Front * 10.0f;
            orbitRadius = glm::length(Position - orbitCenter);
            orbitAngle = 0.0f;
        }
    }

    void UpdateOrbit(float deltaTime) {
        if (orbitMode) {
            orbitAngle += 30.0f * deltaTime;
            if (orbitAngle > 360.0f) orbitAngle -= 360.0f;

            Position.x = orbitCenter.x + orbitRadius * cos(glm::radians(orbitAngle));
            Position.z = orbitCenter.z + orbitRadius * sin(glm::radians(orbitAngle));

            Front = glm::normalize(orbitCenter - Position);
            Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));

        glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Roll), Front);
        glm::vec3 baseUp = glm::normalize(glm::cross(Right, Front));
        Up = glm::vec3(rollMatrix * glm::vec4(baseUp, 0.0f));
    }
};

#endif