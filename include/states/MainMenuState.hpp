#pragma once
#include "states/State.hpp"
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventListener.h>

namespace Rml {
class ElementDocument;
}

class MainMenuState : public State, public Rml::EventListener {
  public:
    void OnEnter(Application* app) override;
    void OnExit(Application* app) override;
    void Update(float deltaTime, Application* app) override;
    void Render(Application* app) override;

    void ProcessEvent(Rml::Event& event) override;

    void ShowMainMenu();
    void ShowLobbyBrowser();

  private:
    Application* m_App = nullptr;
    Rml::ElementDocument* m_Document = nullptr;
    Rml::ElementDocument* m_LobbyBrowser = nullptr;
    // We don't own the document, RmlUi context does, but we can keep a handle
    // if needed or just query it
};
