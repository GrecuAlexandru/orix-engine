#pragma once

#include <SDL.h>
#include <glad/glad.h>
#include <memory>

#include "Camera.hpp"
#include "renderer/Shader.hpp"

class Application {
  public:
    Application();
    ~Application();

    int Run();

  private:
    bool Initialize();
    void ProcessEvents();
    void Update(float deltaTime);
    void Render();
    void Cleanup();

    // Windowing
    SDL_Window *m_Window;
    SDL_GLContext m_GLContext;
    bool m_Running;
    bool m_IsMouseLocked;

    // Camera
    Camera m_Camera;
    float m_LastFrame;

    // Rendering Resources
    std::unique_ptr<Shader> m_BasicShader;
    unsigned int m_VBO, m_VAO;

    // Window Constants
    const int m_WindowWidth = 1280;
    const int m_WindowHeight = 720;
};