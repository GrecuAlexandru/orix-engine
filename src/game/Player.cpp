#include "game/Player.hpp"

Player::Player() {
    Position = glm::vec3(8.0f, 30.0f, 8.0f);
}

void Player::Update(float deltaTime, World& world) {
    HandleMovement(deltaTime, world);

    // Always update camera position to match player
    m_Camera.Position = Position + glm::vec3(0.0f, Height, 0.0f);
}

void Player::UpdateCameraRotation(float deltaTime) {
    // Only update camera rotation (mouse look)
    m_Camera.Update(deltaTime);
}

void Player::HandleMovement(float deltaTime, World& world) {
    // 1. Apply Gravity
    if (!IsGrounded) {
        Velocity.y += GRAVITY * deltaTime;
    } else if (Velocity.y < 0) {
        Velocity.y = -0.1f; // Small downward force to stay glued to slopes
    }

    // 2. Horizontal Input
    glm::vec3 front =
        glm::normalize(glm::vec3(m_Camera.Front.x, 0, m_Camera.Front.z));
    glm::vec3 right =
        glm::normalize(glm::vec3(m_Camera.Right.x, 0, m_Camera.Right.z));

    glm::vec3 moveDir(0.0f);
    if (Input::IsKeyDown(SDL_SCANCODE_W))
        moveDir += front;
    if (Input::IsKeyDown(SDL_SCANCODE_S))
        moveDir -= front;
    if (Input::IsKeyDown(SDL_SCANCODE_A))
        moveDir -= right;
    if (Input::IsKeyDown(SDL_SCANCODE_D))
        moveDir += right;

    if (glm::length(moveDir) > 0)
        moveDir = glm::normalize(moveDir);

    // Set X and Z velocity based on input
    Velocity.x = moveDir.x * Speed;
    Velocity.z = moveDir.z * Speed;

    // 3. Jumping
    if (IsGrounded && Input::IsKeyDown(SDL_SCANCODE_SPACE)) {
        Velocity.y = JumpForce;
        IsGrounded = false;
    }

    // 4. Collision Detection (Simplified Voxel Collision)
    glm::vec3 nextPos = Position + Velocity * deltaTime;

    // A simple "Feet Check": Look at the block below nextPos
    // We convert the float position to integer block coordinates
    int bx = (int)floor(nextPos.x);
    int by = (int)floor(nextPos.y); // Floor level
    int bz = (int)floor(nextPos.z);

    // Check the block at our feet
    if (world.GetBlockAt(bx, by, bz).id != BlockType::Air) {
        // We hit the ground!
        Velocity.y = 0;
        IsGrounded = true;
        // Keep the player on top of the block
        Position.y = (float)by + 1.0f;
    } else {
        IsGrounded = false;
        Position.y = nextPos.y;
    }

    // Apply horizontal movement (ignoring wall collisions for now)
    Position.x = nextPos.x;
    Position.z = nextPos.z;
}