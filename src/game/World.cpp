#include "game/World.hpp"
#include "renderer/Mesh.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

World::World() {}

World::~World() {}

void World::Init() {
    // Create one test chunk at the origin
    Chunk* initialChunk = new Chunk(glm::ivec3(0, 0, 0));
    initialChunk->GenerateMesh();
    m_Chunks[glm::ivec3(0, 0, 0)] = initialChunk;
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