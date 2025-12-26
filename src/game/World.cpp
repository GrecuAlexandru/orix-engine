#include "game/World.hpp"
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
}