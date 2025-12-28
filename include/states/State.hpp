#pragma once

class Application; // Forward declaration

class State {
  public:
    virtual ~State() = default;

    virtual void OnEnter(Application* app) {}
    virtual void OnExit(Application* app) {}
    virtual void Update(float deltaTime, Application* app) {}
    virtual void Render(Application* app) {}
};
