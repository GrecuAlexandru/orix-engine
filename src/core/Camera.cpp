#include "core/Camera.hpp"
#include "core/Input.hpp"

Camera::Camera()
    : Position(glm::vec3(0.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      Yaw(-90.0f), Pitch(0.0f), MovementSpeed(2.5f), MouseSensitivity(0.1f),
      Zoom(45.0f) {
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix(float width, float height) const {
    return glm::perspective(glm::radians(Zoom), width / height, 0.1f, 1000.0f);
}

glm::vec3 Camera::GetPosition() const {
    return Position;
}

void Camera::Update(float deltaTime) {
    // Handle Mouse Rotation
    glm::vec2 mouseDelta = Input::GetMouseDelta();

    Yaw += mouseDelta.x * MouseSensitivity;
    Pitch -= mouseDelta.y * MouseSensitivity;

    // Constrain Pitch so the screen doesn't flip
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    // Re-calculate Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}