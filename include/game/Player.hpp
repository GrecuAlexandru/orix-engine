#pragma once
#include "core/Camera.hpp"
#include "core/Input.hpp"
#include "game/World.hpp"
#include <glm/glm.hpp>

class Player {
  public:
    Player();

    void Update(float deltaTime, World& world);
    void UpdateCameraRotation(float deltaTime);

    Camera& GetCamera() {
        return m_Camera;
    }

    glm::vec3 Position;
    glm::vec3 Velocity = glm::vec3(0.0f);

    bool IsGrounded = false;
    float Height = 1.93f;
    float Speed = 5.0f;
    float JumpForce = 5.0f;
    const float GRAVITY = -15.0f;

  private:
    Camera m_Camera;
    void HandleMovement(float deltaTime, World& world);
};