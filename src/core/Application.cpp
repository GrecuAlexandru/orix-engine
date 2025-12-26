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
    // Receive network packets and interpolate remote players
    Steam::ReceivePackets();
    Steam::InterpolatePlayers(deltaTime);

    // Always update player physics and camera position
    m_Player.Update(deltaTime, m_World);

    if (m_IsMouseLocked) {
        m_Player.UpdateCameraRotation(deltaTime);
    }

    static float networkTimer = 0.0f;
    const float tickInterval = 1.0f / 30.0f; // 30Hz

    networkTimer += deltaTime;
    if (networkTimer >= tickInterval) {
        Steam::SendPosition(m_Player.Position, m_Player.GetCamera().Front);
        networkTimer = 0.0f;
    }
}

void Application::Render() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render World
    m_World.Render(
        *m_BasicShader, m_Player.GetCamera(), m_WindowWidth, m_WindowHeight);

    // Render UI
    RenderUI();

    SDL_GL_SwapWindow(m_Window);
}

void Application::RenderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Debug Window
    ImGui::Begin("Debug",
                 nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoBackground);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Position: %.1f, %.1f, %.1f",
                m_Player.GetCamera().GetPosition().x,
                m_Player.GetCamera().GetPosition().y,
                m_Player.GetCamera().GetPosition().z);

    if (ImGui::Button("Host Steam Lobby")) {
        Steam::CreateLobby();
    }

    ImGui::SameLine();

    if (ImGui::Button("Find & Join Lobby")) {
        Steam::FindLobbies();
    }

    ImGui::Separator();

    CSteamID currentLobby = Steam::GetCurrentLobbyID();
    if (currentLobby.IsValid()) {
        ImGui::Text("Current Lobby ID: %llu", currentLobby.ConvertToUint64());
        if (ImGui::Button("Copy Lobby ID")) {
            SDL_SetClipboardText(
                std::to_string(currentLobby.ConvertToUint64()).c_str());
        }
    }

    ImGui::Text("Join by Lobby ID:");
    ImGui::InputText("##LobbyID", m_LobbyIdInput, sizeof(m_LobbyIdInput));
    ImGui::SameLine();
    if (ImGui::Button("Join")) {
        if (strlen(m_LobbyIdInput) > 0) {
            uint64 lobbyId = std::stoull(m_LobbyIdInput);
            CSteamID steamLobbyId(lobbyId);
            Steam::JoinLobby(steamLobbyId);
        }
    }

    if (!m_IsMouseLocked) {
        ImGui::Text("Press ESC to return to game");
    }

    // Crosshair
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 center = ImVec2(m_WindowWidth / 2.0f, m_WindowHeight / 2.0f);
    float lineSize = 10.0f;
    float thickness = 2.0f;
    ImU32 white = IM_COL32(255, 255, 255, 255);

    drawList->AddLine(ImVec2(center.x - lineSize, center.y),
                      ImVec2(center.x + lineSize, center.y),
                      white,
                      thickness);
    drawList->AddLine(ImVec2(center.x, center.y - lineSize),
                      ImVec2(center.x, center.y + lineSize),
                      white,
                      thickness);
    ImGui::End();

    // Network Debug Window
    ImGui::Begin("Network Debug");

    static float timer = 0;
    static int displayTickrate = 0;

    timer += ImGui::GetIO().DeltaTime;
    if (timer >= 1.0f) {
        displayTickrate = Steam::GetAndResetPacketCount();
        timer = 0;
    }

    ImGui::Text("Incoming Tickrate: %d Hz", displayTickrate);
    ImGui::Separator();

    for (auto const& [id, data] : Steam::RemotePlayers) {
        int ping = Steam::GetPing(id);
        ImGui::Text("Player [%llu]: Ping %dms", id, ping);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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