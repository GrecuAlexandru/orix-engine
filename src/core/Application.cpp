#include "core/Application.hpp"

#include "core/Input.hpp"
#include "game/World.hpp"
#include "platform/Steam.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

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

    // ImGui Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(m_Window, m_GLContext);
    ImGui_ImplOpenGL3_Init("#version 450");

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
        Render();
    }

    return 0;
}

void Application::ProcessEvents() {
    // Update input BEFORE processing events to capture previous frame state
    Input::Update();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT)
            m_Running = false;
    }

    if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
        m_IsMouseLocked = !m_IsMouseLocked;
        Input::SetCursorLock(m_IsMouseLocked);
    }

    if (Input::IsMouseButtonPressed(SDL_BUTTON_LEFT) && !m_IsMouseLocked) {
        if (!ImGui::GetIO().WantCaptureMouse) {
            m_IsMouseLocked = true;
            Input::SetCursorLock(m_IsMouseLocked);
        }
    }
}

void Application::Update(float deltaTime) {
    // Always update player physics and camera position
    m_Player.Update(deltaTime, m_World);

    // Only allow camera rotation when mouse is locked
    if (m_IsMouseLocked) {
        m_Player.UpdateCameraRotation(deltaTime);
    }
}

void Application::Render() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render World
    m_World.Render(
        *m_BasicShader, m_Player.GetCamera(), m_WindowWidth, m_WindowHeight);

    // UI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug",
                 nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoBackground);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Position: %.1f, %.1f, %.1f",
                m_Player.GetCamera().GetPosition().x,
                m_Player.GetCamera().GetPosition().y,
                m_Player.GetCamera().GetPosition().z);
    // Draw Crosshair
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 center = ImVec2(m_WindowWidth / 2.0f, m_WindowHeight / 2.0f);
    float lineSize = 10.0f;
    float thickness = 2.0f;
    ImU32 white = IM_COL32(255, 255, 255, 255);

    // Horizontal line
    drawList->AddLine(ImVec2(center.x - lineSize, center.y),
                      ImVec2(center.x + lineSize, center.y),
                      white,
                      thickness);
    // Vertical line
    drawList->AddLine(ImVec2(center.x, center.y - lineSize),
                      ImVec2(center.x, center.y + lineSize),
                      white,
                      thickness);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(m_Window);
}

void Application::Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(m_GLContext);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
    Steam::Shutdown();
}