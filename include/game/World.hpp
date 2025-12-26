#pragma once

#include "core/Camera.hpp"
#include "renderer/Shader.hpp"

#include "Chunk.hpp"
#include <glm/glm.hpp>
#include <map>

// Custom comparator for glm::ivec3 to use as map key
struct IVec3Compare {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        if (a.x != b.x)
            return a.x < b.x;
        if (a.y != b.y)
            return a.y < b.y;
        return a.z < b.z;
    }
};

class World {
  public:
    World();
    ~World();

    void Init();
    void Update(float deltaTime);
    void Render(Shader& shader, const Camera& camera, int width, int height);

  private:
    std::map<glm::ivec3, Chunk*, IVec3Compare> m_Chunks;
};