#pragma once
#include "Block.hpp"
#include "renderer/Shader.hpp"
#include <glm/glm.hpp>
#include <vector>

const int CHUNK_SIZE = 16;

class Chunk {
  public:
    Chunk(glm::ivec3 position);
    ~Chunk();

    void GenerateMesh();
    void Render(Shader& shader);

    void SetBlock(int x, int y, int z, BlockType type);
    Block GetBlock(int x, int y, int z);

  private:
    glm::ivec3 m_WorldPos;
    Block m_Blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    unsigned int m_VAO, m_VBO;
    int m_VertexCount;

    void addFace(std::vector<float>& vertices, glm::vec3 pos, int faceIndex);
};