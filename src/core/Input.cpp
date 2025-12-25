#include "core/Input.hpp"

const uint8_t *Input::m_KeyboardState = nullptr;
uint8_t Input::m_PrevKeyboardState[SDL_NUM_SCANCODES] = {0};
uint32_t Input::m_MouseState = 0;
uint32_t Input::m_PrevMouseState = 0;
glm::vec2 Input::m_MouseDelta = glm::vec2(0.0f);

void Input::Update() {
    // Get current keyboard state first
    const uint8_t *currentKeyboardState = SDL_GetKeyboardState(NULL);

    // Store previous state (copy from the previous current state)
    if (m_KeyboardState) {
        memcpy(m_PrevKeyboardState, m_KeyboardState, SDL_NUM_SCANCODES);
    }

    // Update to new current state
    m_KeyboardState = currentKeyboardState;
    m_PrevMouseState = m_MouseState;

    // Get mouse state and position
    int x, y;
    m_MouseState = SDL_GetRelativeMouseState(&x, &y);
    m_MouseDelta = glm::vec2((float)x, (float)y);
}

bool Input::IsKeyDown(SDL_Scancode key) {
    return m_KeyboardState && m_KeyboardState[key];
}

bool Input::IsKeyPressed(SDL_Scancode key) {
    return (m_KeyboardState && m_KeyboardState[key]) &&
           !m_PrevKeyboardState[key];
}

bool Input::IsMouseButtonDown(uint8_t button) {
    return (m_MouseState & SDL_BUTTON(button)) != 0;
}

bool Input::IsMouseButtonPressed(uint8_t button) {
    return ((m_MouseState & SDL_BUTTON(button)) != 0) &&
           ((m_PrevMouseState & SDL_BUTTON(button)) == 0);
}

glm::vec2 Input::GetMouseDelta() { return m_MouseDelta; }

void Input::SetCursorLock(bool locked) {
    SDL_SetRelativeMouseMode(locked ? SDL_TRUE : SDL_FALSE);
}