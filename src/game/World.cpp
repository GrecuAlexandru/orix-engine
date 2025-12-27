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

    // Bind the player cube VAO for rendering remote players
    glBindVertexArray(m_PlayerCubeVAO);

    // Render remote players
    for (auto const& [id, data] : Steam::RemotePlayers) {
        // Render body (rectangular box that rotates with yaw)
        glm::mat4 bodyModel = glm::mat4(1.0f);
        bodyModel = glm::translate(bodyModel, data.currentPos);
        bodyModel = glm::translate(
            bodyModel,
            glm::vec3(0.0f, 0.6f, 0.0f)); // Lift body so bottom is at feet
        bodyModel = glm::rotate(
            bodyModel, glm::radians(-(data.yaw - 90.0f)), glm::vec3(0, 1, 0));
        bodyModel = glm::scale(bodyModel, glm::vec3(0.6f, 1.2f, 0.4f));

        shader.SetMat4("u_Model", bodyModel);
        shader.SetVec3("u_Color", glm::vec3(0.2f, 0.4f, 1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render head (cube positioned above body, rotates with yaw and tilts
        // with pitch)
        glm::mat4 headModel = glm::mat4(1.0f);
        headModel = glm::translate(headModel, data.currentPos);
        headModel = glm::translate(
            headModel,
            glm::vec3(
                0.0f,
                1.6f,
                0.0f)); // Position head above body (body height 1.2 + 0.4)
        headModel = glm::rotate(headModel,
                                glm::radians(-(data.yaw - 90.0f)),
                                glm::vec3(0, 1, 0)); // Rotate with body
        headModel = glm::rotate(headModel,
                                glm::radians(-data.pitch),
                                glm::vec3(1, 0, 0)); // Tilt up/down
        headModel = glm::scale(headModel, glm::vec3(0.4f, 0.4f, 0.4f));

        shader.SetMat4("u_Model", headModel);
        shader.SetVec3("u_Color", glm::vec3(1.0f, 0.8f, 0.6f));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Unbind VAO
    glBindVertexArray(0);
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