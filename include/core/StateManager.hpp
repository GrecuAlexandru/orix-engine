#pragma once
#include "states/State.hpp"
#include <memory>
#include <vector>

class Application;

class StateManager {
  public:
    explicit StateManager(Application* app);

    void PushState(std::unique_ptr<State> state);
    void PopState();
    void ChangeState(std::unique_ptr<State> state);

    void Update(float deltaTime);
    void Render();

  private:
    std::vector<std::unique_ptr<State>> m_States;
    Application* m_App;
};
