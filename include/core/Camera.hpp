#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
  public:
    Camera();

    // Perspective / View Matrices
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float width, float height) const;
    glm::vec3 GetPosition() const;

    // High-level update functions
    void Update(float deltaTime);

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera Options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

  private:
    void updateCameraVectors();
};