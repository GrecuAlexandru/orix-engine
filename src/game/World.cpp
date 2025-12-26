#include "game/World.hpp"
#include "platform/Steam.hpp"
#include "renderer/Mesh.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

World::World() {}

World::~World() {}

void World::Init() {
    for (int x = 0; x < 4; x++) {
        for (int z = 0; z < 4; z++) {
            glm::ivec3 chunkPos(x * CHUNK_SIZE, 0, z * CHUNK_SIZE);
            Chunk* newChunk = new Chunk(chunkPos);
            newChunk->GenerateMesh();
            m_Chunks[chunkPos] = newChunk;
        }
    }

    InitPlayerCube();
}

void World::Update(float deltaTime) {
    // Future: Handle block placing/breaking or day/night cycle
}

void World::Render(Shader& shader,
                   const Camera& camera,
                   int width,
                   int height) {
    shader.Use();
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection =
        camera.GetProjectionMatrix((float)width, (float)height);

    // We send View and Projection once. Model is sent per chunk inside
    // Chunk::Render.
    shader.SetMat4("u_VP", projection * view);

    for (auto const& [pos, chunk] : m_Chunks) {
        chunk->Render(shader);
    }

    // Render remote players
    for (auto const& [id, data] : Steam::RemotePlayers) {
        // 1. Draw the Player Body (Cube) - using smooth interpolated position
        glm::mat4 model = glm::translate(glm::mat4(1.0f), data.currentPos);
        // Scale to player size (roughly 2 blocks tall, 1 block wide)
        model = glm::scale(model, glm::vec3(0.8f, 1.8f, 0.8f));
        shader.SetMat4("u_Model", model);

        glBindVertexArray(m_PlayerCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2. Draw a "Direction Indicator" (A smaller cube in front of the face)
        glm::vec3 indicatorPos = data.currentPos + (data.direction * 0.4f);
        glm::mat4 indicatorModel =
            glm::translate(glm::mat4(1.0f), indicatorPos);
        indicatorModel =
            glm::scale(indicatorModel, glm::vec3(0.3f)); // Make it smaller

        shader.SetMat4("u_Model", indicatorModel);
        glBindVertexArray(m_PlayerCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

Block World::GetBlockAt(int x, int y, int z) {
    // 1. Find which chunk this coordinate belongs to
    // (Divide by 16 and floor)
    glm::ivec3 chunkCoord((x >= 0) ? x / 16 : (x - 15) / 16,
                          (y >= 0) ? y / 16 : (y - 15) / 16,
                          (z >= 0) ? z / 16 : (z - 15) / 16);

    // Scale back up to get the chunk's world origin
    chunkCoord *= 16;

    if (m_Chunks.find(chunkCoord) != m_Chunks.end()) {
        // 2. Find local coordinates inside that chunk (0-15)
        int lx = x - chunkCoord.x;
        int ly = y - chunkCoord.y;
        int lz = z - chunkCoord.z;
        return m_Chunks[chunkCoord]->GetBlock(lx, ly, lz);
    }

    return Block(BlockType::Air); // If chunk doesn't exist, it's air
}

void World::InitPlayerCube() {
    // Simple cube vertices (position only, 36 vertices for 6 faces)
    float vertices[] = {// Back face
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        // Front face
                        -0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        // Left face
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        // Right face
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        // Bottom face
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f,
                        -0.5f,
                        -0.5f,
                        // Top face
                        -0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        0.5f,
                        -0.5f,
                        0.5f,
                        -0.5f};

    glGenVertexArrays(1, &m_PlayerCubeVAO);
    glGenBuffers(1, &m_PlayerCubeVBO);

    glBindVertexArray(m_PlayerCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PlayerCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}