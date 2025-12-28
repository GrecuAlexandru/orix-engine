#pragma once
#include "states/State.hpp"

class PlayState : public State {
  public:
    void OnEnter(Application* app) override;
    void OnExit(Application* app) override;
    void Update(float deltaTime, Application* app) override;
    void Render(Application* app) override;
};
