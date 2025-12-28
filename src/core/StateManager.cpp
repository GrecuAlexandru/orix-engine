#include "core/StateManager.hpp"

StateManager::StateManager(Application* app) : m_App(app) {}

void StateManager::PushState(std::unique_ptr<State> state) {
    m_States.push_back(std::move(state));
    m_States.back()->OnEnter(m_App);
}

void StateManager::PopState() {
    if (!m_States.empty()) {
        m_States.back()->OnExit(m_App);
        m_States.pop_back();
    }
}

void StateManager::ChangeState(std::unique_ptr<State> state) {
    if (!m_States.empty()) {
        m_States.back()->OnExit(m_App);
        m_States.pop_back();
    }
    m_States.push_back(std::move(state));
    m_States.back()->OnEnter(m_App);
}

void StateManager::Update(float deltaTime) {
    if (!m_States.empty()) {
        m_States.back()->Update(deltaTime, m_App);
    }
}

void StateManager::Render() {
    if (!m_States.empty()) {
        m_States.back()->Render(m_App);
    }
}
