#pragma once

#include <glad/glad.h>
#include <SDL.h>
#include <memory>

#include "game/Player.hpp"
#include "game/World.hpp"
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
    void RenderUI();
    void Cleanup();

    // Windowing
    SDL_Window* m_Window;
    SDL_GLContext m_GLContext;
    bool m_Running;
    bool m_IsMouseLocked;

    // Rendering Resources
    std::unique_ptr<Shader> m_BasicShader;

    // Window Constants
    const int m_WindowWidth = 1280 / 2;
    const int m_WindowHeight = 720 / 2;

    // World
    World m_World;

    // Player
    Player m_Player;
    float m_LastFrame;

    // Steam lobby ID input
    char m_LobbyIdInput[64] = "";
};