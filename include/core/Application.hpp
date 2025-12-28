#pragma once

#include <glad/glad.h>
#include <SDL.h>
#include <memory>

#include "core/StateManager.hpp"
#include "game/Player.hpp"
#include "game/World.hpp"
#include "renderer/Shader.hpp"
#include "ui/UIManager.hpp"

enum class GameState {
    MainMenu,
    InGame
};

class Application {
  public:
    Application();
    ~Application();

    int Run();
    void EnterGame(); // Transition from menu to game

    // Getters for States
    World& GetWorld() {
        return m_World;
    }
    Player& GetPlayer() {
        return m_Player;
    }
    Shader& GetShader() {
        return *m_BasicShader;
    }
    UIManager* GetUIManager() {
        return m_UIManager.get();
    }

    int GetWidth() const {
        return m_WindowWidth;
    }
    int GetHeight() const {
        return m_WindowHeight;
    }
    float GetNetworkTickrate() const {
        return m_NetworkTickrate;
    }

    bool IsMouseLocked() const {
        return m_IsMouseLocked;
    }
    void SetMouseLocked(bool locked);

    void PushState(std::unique_ptr<State> state) {
        m_StateManager->PushState(std::move(state));
    }
    void ChangeState(std::unique_ptr<State> state) {
        m_StateManager->ChangeState(std::move(state));
    }

  private:
    bool Initialize();
    void ProcessEvents();
    void Update(float deltaTime);
    void Render();
    void Cleanup();

    // Managers
    std::unique_ptr<StateManager> m_StateManager;
    std::unique_ptr<UIManager> m_UIManager;

    // Defer state changes to avoid crashing RmlUi during event processing
    std::unique_ptr<State> m_PendingState;

    // Windowing
    SDL_Window* m_Window;
    SDL_GLContext m_GLContext;
    bool m_Running;
    bool m_IsMouseLocked;

    // Rendering Resources
    std::unique_ptr<Shader> m_BasicShader;

    // Window Constants
    const int m_WindowWidth = 871;
    const int m_WindowHeight = 490;

    // World
    World m_World;

    // Player
    Player m_Player;
    float m_LastFrame;

    // Steam lobby ID input
    char m_LobbyIdInput[64] = "";

    float m_NetworkTickrate = 30.0f;
};