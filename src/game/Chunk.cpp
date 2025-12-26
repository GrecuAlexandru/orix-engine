#include "game/Chunk.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "game/FastNoiseLite.h"

Chunk::Chunk(glm::ivec3 position)
    : m_WorldPos(position), m_VAO(0), m_VBO(0), m_VertexCount(0) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.05f);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float worldX = (float)(m_WorldPos.x + x);
            float worldZ = (float)(m_WorldPos.z + z);

            float noiseValue = noise.GetNoise(worldX, worldZ);
            int height = (int)((noiseValue + 1.0f) * 8.0f);

            for (int y = 0; y < height; y++) {
                if (y == height - 1) {
                    // Top block is grass
                    m_Blocks[x][y][z] = Block(BlockType::Grass);
                } else if (y >= height - 4) {
                    // Next 3 blocks down are dirt
                    m_Blocks[x][y][z] = Block(BlockType::Dirt);
                } else {
                    // Everything below is stone
                    m_Blocks[x][y][z] = Block(BlockType::Stone);
                }
            }
        }
    }
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Chunk::GenerateMesh() {
    std::vector<float> vertices;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (m_Blocks[x][y][z].id == BlockType::Air)
                    continue;

                glm::vec3 pos(x, y, z);
                float blockID = static_cast<float>(m_Blocks[x][y][z].id);

                // Face culling

                // 0: Back (-Z)
                if (z == 0 || m_Blocks[x][y][z - 1].id == BlockType::Air)
                    addFace(vertices, pos, 0, blockID);

                // 1: Front (+Z)
                if (z == CHUNK_SIZE - 1 ||
                    m_Blocks[x][y][z + 1].id == BlockType::Air)
                    addFace(vertices, pos, 1, blockID);

                // 2: Left (-X)
                if (x == 0 || m_Blocks[x - 1][y][z].id == BlockType::Air)
                    addFace(vertices, pos, 2, blockID);

                // 3: Right (+X)
                if (x == CHUNK_SIZE - 1 ||
                    m_Blocks[x + 1][y][z].id == BlockType::Air)
                    addFace(vertices, pos, 3, blockID);

                // 4: Bottom (-Y)
                if (y == 0 || m_Blocks[x][y - 1][z].id == BlockType::Air)
                    addFace(vertices, pos, 4, blockID);

                // 5: Top (+Y)
                if (y == CHUNK_SIZE - 1 ||
                    m_Blocks[x][y + 1][z].id == BlockType::Air)
                    addFace(vertices, pos, 5, blockID);
            }
        }
    }

    m_VertexCount =
        vertices.size() / 6; // Now 6 floats per vertex (pos xyz, uv, blockID)

    if (m_VAO == 0)
        glGenVertexArrays(1, &m_VAO);
    if (m_VBO == 0)
        glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (location 1)
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Block ID attribute (location 2)
    glVertexAttribPointer(2,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void Chunk::Render(Shader& shader) {
    if (m_VertexCount == 0)
        return;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_WorldPos));
    shader.SetMat4("u_Model", model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
}

// Helper function to keep code clean
void Chunk::addFace(std::vector<float>& vertices,
                    glm::vec3 p,
                    int face,
                    float blockID) {
    // clang-format off
    // Face data: 6 vertices per face, 5 floats per vertex (Pos X,Y,Z, U,V)
    // Each face is 2 triangles = 6 vertices
    const float faceData[6][30] = {
        // Back face (face 0)
        {-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f},

        // Front face (face 1)
        {-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f},

        // Left face (face 2)
        {-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f},

        // Right face (face 3)
        { 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f},

        // Bottom face (face 4)
        {-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f},

        // Top face (face 5)
        {-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f}
    };
    // clang-format on

    // Add the face vertices, offset by the block position
    for (int i = 0; i < 30; i += 5) {
        vertices.push_back(faceData[face][i + 0] + p.x); // X position
        vertices.push_back(faceData[face][i + 1] + p.y); // Y position
        vertices.push_back(faceData[face][i + 2] + p.z); // Z position
        vertices.push_back(faceData[face][i + 3]);       // U coord
        vertices.push_back(faceData[face][i + 4]);       // V coord
        vertices.push_back(blockID);                     // Block ID
    }
}

void Chunk::SetBlock(int x, int y, int z, BlockType type) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 &&
        z < CHUNK_SIZE) {
        m_Blocks[x][y][z] = Block(type);
    }
}

Block Chunk::GetBlock(int x, int y, int z) {
    if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 &&
        z < CHUNK_SIZE) {
        return m_Blocks[x][y][z];
    }
    return Block(BlockType::Air);
}