#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Shader {
  public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);

    void Use();

    void SetMat4(const std::string& name, const glm::mat4& mat) const;
    void SetVec3(const std::string& name, const glm::vec3& vec) const;
    void SetFloat(const std::string& name, float value) const;

  private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
