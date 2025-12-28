#pragma once
#include <RmlUi/Core.h>
#include <SDL.h>
#include <memory>


class RenderInterface_GL3;
class SystemInterface_SDL;

class UIManager {
  public:
    UIManager();
    ~UIManager();

    bool Initialize(SDL_Window* window, int width, int height);
    void Shutdown();

    void ProcessEvent(SDL_Event& event, SDL_Window* window);
    void Update();
    void BeginFrame();
    void EndFrame();
    void Render();

    Rml::Context* GetRmlContext() const {
        return m_RmlContext;
    }

  private:
    std::unique_ptr<RenderInterface_GL3> m_RmlRenderer;
    std::unique_ptr<SystemInterface_SDL> m_RmlSystem;
    Rml::Context* m_RmlContext = nullptr;
};
