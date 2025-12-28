#include "core/Application.hpp"
#include "core/Input.hpp"
#include "platform/Steam.hpp"
#include "states/MainMenuState.hpp"
#include "states/PlayState.hpp"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#undef GetUserName
#endif

Application::Application()
    : m_Window(nullptr), m_GLContext(nullptr), m_Running(true),
      m_IsMouseLocked(false), m_LastFrame(0.0f) {}

Application::~Application() {
    Cleanup();
}

bool Application::Initialize() {
    if (!Steam::Init())
        return false;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        return false;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    m_Window = SDL_CreateWindow("Orix Engine",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                m_WindowWidth,
                                m_WindowHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_GLContext = SDL_GL_CreateContext(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        return false;

    // Initialize Managers
    m_UIManager = std::make_unique<UIManager>();
    if (!m_UIManager->Initialize(m_Window, m_WindowWidth, m_WindowHeight)) {
        return false;
    }

    m_StateManager = std::make_unique<StateManager>(this);
    m_StateManager->PushState(std::make_unique<MainMenuState>());

    glEnable(GL_DEPTH_TEST);
    Input::SetCursorLock(m_IsMouseLocked);

    // Load Shaders
    m_BasicShader =
        std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");

    // Initialize World
    m_World.Init();

    return true;
}

int Application::Run() {
    if (!Initialize())
        return -1;

    while (m_Running) {
        float currentFrame = SDL_GetTicks() / 1000.0f;
        float deltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        ProcessEvents();
        Update(deltaTime);
        Steam::Update();
        Render();
    }

    return 0;
}

void Application::ProcessEvents() {
    // Update input BEFORE processing events to capture previous frame state
    Input::Update();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        m_UIManager->ProcessEvent(e, m_Window);
        if (e.type == SDL_QUIT)
            m_Running = false;
    }

    if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
        // Toggle mouse lock or return to menu logic could go here
        // For now, let's keep it simple or delegate to state
        // m_IsMouseLocked = !m_IsMouseLocked;
        // SetMouseLocked(m_IsMouseLocked);
    }
}

void Application::Update(float deltaTime) {
    if (m_PendingState) {
        m_StateManager->ChangeState(std::move(m_PendingState));
    }

    m_UIManager->Update();
    m_StateManager->Update(deltaTime);
}

void Application::Render() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start ImGui frame BEFORE state render (state may use ImGui)
    m_UIManager->BeginFrame();

    // Render world/game content
    m_StateManager->Render();

    // Render RmlUi and finish ImGui
    m_UIManager->Render();
    m_UIManager->EndFrame();

    SDL_GL_SwapWindow(m_Window);
}

void Application::EnterGame() {
    m_PendingState = std::make_unique<PlayState>();
}

void Application::SetMouseLocked(bool locked) {
    m_IsMouseLocked = locked;
    Input::SetCursorLock(locked);
    // SDL_SetRelativeMouseMode is handled in Input::SetCursorLock or should be
}

void Application::Cleanup() {
    m_StateManager = nullptr;
    m_UIManager = nullptr;
    SDL_GL_DeleteContext(m_GLContext);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
    Steam::Shutdown();
}