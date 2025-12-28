#include "ui/UIManager.hpp"
#include <RmlUi_Platform_SDL.h>
#include <RmlUi_Renderer_GL3.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

#ifdef _WIN32
#include <windows.h>
#undef GetUserName
#endif

UIManager::UIManager() {}

UIManager::~UIManager() {
    Shutdown();
}

bool UIManager::Initialize(SDL_Window* window, int width, int height) {
    // ImGui Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, SDL_GL_GetCurrentContext());
    ImGui_ImplOpenGL3_Init("#version 450");

    // RmlUi Init
    m_RmlRenderer = std::make_unique<RenderInterface_GL3>();
    m_RmlSystem = std::make_unique<SystemInterface_SDL>();

    Rml::SetRenderInterface(m_RmlRenderer.get());
    Rml::SetSystemInterface(m_RmlSystem.get());

    if (!Rml::Initialise()) {
        std::cerr << "[UIManager] Failed to initialize RmlUi" << std::endl;
        return false;
    }

    m_RmlContext = Rml::CreateContext("main", Rml::Vector2i(width, height));
    if (!m_RmlContext) {
        std::cerr << "[UIManager] Failed to create RmlUi context" << std::endl;
        return false;
    }

    m_RmlRenderer->SetViewport(width, height);
    m_RmlSystem->SetWindow(window);

    // Load font
    if (!Rml::LoadFontFace("assets/fonts/PressStart2P-Regular.ttf")) {
        std::cerr << "[UIManager] Failed to load font: "
                     "assets/fonts/PressStart2P-Regular.ttf"
                  << std::endl;
    }

    return true;
}

void UIManager::Shutdown() {
    if (m_RmlContext) {
        // Shutdown sequence
        // RmlUi Shutdown will destroy the context, but we should clear our
        // pointer
    }

    // We can rely on global ImGui context check
    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    // Only shutdown Rml if it was initialized (hard to check explicitly without
    // flag, but safe to call if not init?) Rml::Shutdown handles internal
    // checks usually, but let's be safe. If m_RmlContext was created,
    // Rml::Initialise definitely succeeded.
    if (m_RmlContext) {
        Rml::Shutdown();
        m_RmlContext = nullptr;
    } else if (m_RmlRenderer) {
        // If we created renderer but failed creating context/doc, we still
        // might need Rml::Shutdown if Initialise succeeded But for now let's
        // assume if context is null, maybe just leave it
        Rml::Shutdown();
    }

    m_RmlRenderer.reset();
    m_RmlSystem.reset();
}

void UIManager::ProcessEvent(SDL_Event& event, SDL_Window* window) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    RmlSDL::InputEventHandler(m_RmlContext, window, event);
}

void UIManager::Update() {
    if (m_RmlContext) {
        m_RmlContext->Update();
    }
}

void UIManager::BeginFrame() {
    // Only start ImGui frame here - RmlUi will start in Render()
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void UIManager::EndFrame() {
    if (m_RmlRenderer) {
        m_RmlRenderer->EndFrame();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Render() {
    // Start RmlUi frame and render
    if (m_RmlRenderer) {
        m_RmlRenderer->BeginFrame();
    }
    if (m_RmlContext) {
        m_RmlContext->Render();
    }
}
