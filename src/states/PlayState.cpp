#include "states/PlayState.hpp"
#include "core/Application.hpp"
#include "platform/Steam.hpp"
#include "ui/UIManager.hpp"
#include "imgui.h"

#include <iostream>

void PlayState::OnEnter(Application* app) {
    app->SetMouseLocked(true);
}

void PlayState::OnExit(Application* app) {
    app->SetMouseLocked(false);
}

void PlayState::Update(float deltaTime, Application* app) {
    // std::cout << "[PlayState] Update" << std::endl; // Too spammy
    // Steam updates
    Steam::ReceivePackets();
    Steam::InterpolatePlayers(deltaTime);

    // Player/World updates
    app->GetPlayer().Update(deltaTime, app->GetWorld());

    if (app->IsMouseLocked()) {
        app->GetPlayer().UpdateCameraRotation(deltaTime);
    }

    // Network tick
    static float networkTimer = 0.0f;
    const float tickInterval = 1.0f / app->GetNetworkTickrate();

    networkTimer += deltaTime;
    if (networkTimer >= tickInterval) {
        auto& player = app->GetPlayer();
        Steam::SendPosition(player.Position, player.Yaw, player.Pitch);
        networkTimer = 0.0f;
    }
}

void PlayState::Render(Application* app) {
    // Reset OpenGL State for 3D rendering
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render World
    app->GetWorld().Render(app->GetShader(),
                           app->GetPlayer().GetCamera(),
                           app->GetWidth(),
                           app->GetHeight());

    // In-Game UI (ImGui Debug)
    ImGui::SetNextWindowBgAlpha(
        0.7f); // Ensure it's visible but semi-transparent
    if (ImGui::Begin(
            "Debug Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        auto& camera = app->GetPlayer().GetCamera();
        ImGui::Text("Position: %.2f, %.2f, %.2f",
                    camera.GetPosition().x,
                    camera.GetPosition().y,
                    camera.GetPosition().z);

        ImGui::Text("Rotation: Pitch %.1f, Yaw %.1f",
                    app->GetPlayer().Pitch,
                    app->GetPlayer().Yaw);

        ImGui::Separator();
        ImGui::Text("Network Tickrate: %.0f Hz", app->GetNetworkTickrate());
        ImGui::End();
    }

    // Crosshair
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 center = ImVec2(app->GetWidth() / 2.0f, app->GetHeight() / 2.0f);
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
}
