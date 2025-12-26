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