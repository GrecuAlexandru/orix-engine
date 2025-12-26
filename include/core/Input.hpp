#pragma once
#include <SDL2/SDL.h>
#include <SDL_scancode.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Input {
  public:
    static void Update();

    // Keyboard
    static bool IsKeyDown(SDL_Scancode key);
    static bool IsKeyPressed(SDL_Scancode key);

    // Mouse
    static bool IsMouseButtonDown(uint8_t button);
    static bool IsMouseButtonPressed(uint8_t button);
    static glm::vec2 GetMouseDelta();
    static void SetCursorLock(bool locked);

  private:
    inline static const uint8_t* m_KeyboardState = nullptr;
    inline static uint8_t m_PrevKeyboardState[SDL_NUM_SCANCODES];
    inline static uint32_t m_MouseState = 0;
    inline static uint32_t m_PrevMouseState = 0;
    inline static glm::vec2 m_MouseDelta = glm::vec2(0.0f);
};