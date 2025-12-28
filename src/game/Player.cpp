#include "game/Player.hpp"

Player::Player() {
    Position = glm::vec3(8.0f, 30.0f, 8.0f);
}

void Player::Update(float deltaTime, World& world) {
    HandleMovement(deltaTime, world);
    m_Camera.Position = Position + glm::vec3(0.0f, Height, 0.0f);
}

void Player::UpdateCameraRotation(float deltaTime) {
    m_Camera.Update(deltaTime);

    // Sync player's yaw and pitch with camera's rotation
    Yaw = m_Camera.Yaw;
    Pitch = m_Camera.Pitch;
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

    // 4. Collision Detection
    glm::vec3 nextPos = Position + Velocity * deltaTime;

    // Player collision box parameters
    const float playerWidth = 0.3f; // Half-width of player hitbox
    const float stepHeight = 1.0f;  // Max height player can auto-step

    // Helper lambda to check if a position collides with solid blocks
    auto collidesWithWorld = [&](float px, float py, float pz) -> bool {
        // Check blocks at feet and head level
        for (float yOffset = 0.0f; yOffset < Height; yOffset += 0.9f) {
            int bx = (int)floor(px);
            int by = (int)floor(py + yOffset);
            int bz = (int)floor(pz);
            Block block = world.GetBlockAt(bx, by, bz);
            if (block.type != BlockType::Air) {
                return true;
            }
        }
        return false;
    };

    // 5. Horizontal Collision with Auto-Step
    float newX = nextPos.x;
    float newZ = nextPos.z;
    float newY = Position.y;

    // Try moving in X direction
    if (Velocity.x != 0.0f) {
        bool canMoveX = !collidesWithWorld(nextPos.x, Position.y, Position.z);

        if (!canMoveX) {
            // Check if we can step up
            bool canStepX = !collidesWithWorld(
                nextPos.x, Position.y + stepHeight, Position.z);
            if (canStepX && IsGrounded) {
                // Find the block height we need to step onto
                int blockX = (int)floor(nextPos.x);
                int blockZ = (int)floor(Position.z);
                for (int checkY = (int)floor(Position.y);
                     checkY <= (int)floor(Position.y + stepHeight);
                     checkY++) {
                    Block b = world.GetBlockAt(blockX, checkY, blockZ);
                    if (b.type != BlockType::Air) {
                        newY = (float)(checkY + 1);
                        canMoveX = true;
                        break;
                    }
                }
            }
        }

        if (canMoveX) {
            newX = nextPos.x;
        } else {
            newX = Position.x;
            Velocity.x = 0;
        }
    }

    // Try moving in Z direction
    if (Velocity.z != 0.0f) {
        bool canMoveZ = !collidesWithWorld(newX, Position.y, nextPos.z);

        if (!canMoveZ) {
            // Check if we can step up
            bool canStepZ =
                !collidesWithWorld(newX, Position.y + stepHeight, nextPos.z);
            if (canStepZ && IsGrounded) {
                // Find the block height we need to step onto
                int blockX = (int)floor(newX);
                int blockZ = (int)floor(nextPos.z);
                for (int checkY = (int)floor(Position.y);
                     checkY <= (int)floor(Position.y + stepHeight);
                     checkY++) {
                    Block b = world.GetBlockAt(blockX, checkY, blockZ);
                    if (b.type != BlockType::Air) {
                        newY = (float)(checkY + 1);
                        canMoveZ = true;
                        break;
                    }
                }
            }
        }

        if (canMoveZ) {
            newZ = nextPos.z;
        } else {
            newZ = Position.z;
            Velocity.z = 0;
        }
    }

    Position.x = newX;
    Position.z = newZ;
    if (newY > Position.y) {
        Position.y = newY; // Apply step-up
    }

    // 6. Vertical Collision (Ground Check)
    nextPos.y = Position.y + Velocity.y * deltaTime;

    int bx = (int)floor(Position.x);
    int by = (int)floor(nextPos.y - 0.1f);
    int bz = (int)floor(Position.z);

    Block blockBelow = world.GetBlockAt(bx, by, bz);

    if (blockBelow.type != BlockType::Air) {
        float blockTopY = (float)by + 1.0f;

        if (nextPos.y <= blockTopY) {
            Position.y = blockTopY;
            Velocity.y = 0;
            IsGrounded = true;
        } else {
            Position.y = nextPos.y;
            IsGrounded = false;
        }
    } else {
        IsGrounded = false;
        Position.y = nextPos.y;
    }
}