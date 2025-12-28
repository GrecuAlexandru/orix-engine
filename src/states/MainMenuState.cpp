#include "states/MainMenuState.hpp"
#include "core/Application.hpp"
#include "platform/Steam.hpp"
#include "ui/UIManager.hpp"
#include <SDL.h>

void MainMenuState::OnEnter(Application* app) {
    m_App = app;

    // Ensure mouse is unlocked
    app->SetMouseLocked(false);

    auto* context = app->GetUIManager()->GetRmlContext();
    if (context) {
        m_Document = context->LoadDocument("assets/gui/main_menu.rml");
        if (m_Document) {
            m_Document->Show();

            if (auto el = m_Document->GetElementById("host"))
                el->AddEventListener(Rml::EventId::Click, this);
            if (auto el = m_Document->GetElementById("join"))
                el->AddEventListener(Rml::EventId::Click, this);
            if (auto el = m_Document->GetElementById("exit"))
                el->AddEventListener(Rml::EventId::Click, this);
        }
    }
}

void MainMenuState::OnExit(Application* app) {

    // IMPORTANT: Remove event listeners BEFORE closing document to avoid
    // dangling pointers
    if (m_Document) {
        if (auto el = m_Document->GetElementById("host"))
            el->RemoveEventListener(Rml::EventId::Click, this);
        if (auto el = m_Document->GetElementById("join"))
            el->RemoveEventListener(Rml::EventId::Click, this);
        if (auto el = m_Document->GetElementById("exit"))
            el->RemoveEventListener(Rml::EventId::Click, this);

        m_Document->Close();
        m_Document = nullptr;
    }
}

void MainMenuState::Update(float deltaTime, Application* app) {
    // Menu logic if any
}

void MainMenuState::Render(Application* app) {
    // Nothing specific to render besides UI which is handled globally
}

void MainMenuState::ProcessEvent(Rml::Event& event) {
    auto target = event.GetTargetElement();
    if (!target)
        return;
    std::string id = target->GetId();

    if (id == "host") {
        Steam::CreateLobby();
        m_App->EnterGame();
    } else if (id == "join") {
        // Search for friend lobbies and show browser
        Steam::FindFriendLobbies();
        ShowLobbyBrowser();
    } else if (id == "exit") {
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    } else if (id == "back") {
        // Return from lobby browser to main menu
        ShowMainMenu();
    } else if (id.find("join_lobby_") == 0) {
        // Extract lobby index from button ID (e.g., "join_lobby_0")
        int lobbyIndex = std::stoi(id.substr(11));
        if (lobbyIndex >= 0 && lobbyIndex < (int)Steam::FoundLobbies.size()) {
            Steam::JoinLobby(Steam::FoundLobbies[lobbyIndex].id);
            m_App->EnterGame();
        }
    }
}

void MainMenuState::ShowMainMenu() {
    auto* context = m_App->GetUIManager()->GetRmlContext();
    if (!context)
        return;

    // Hide lobby browser if open
    if (m_LobbyBrowser) {
        m_LobbyBrowser->Close();
        m_LobbyBrowser = nullptr;
    }

    // Show main menu
    if (m_Document) {
        m_Document->Show();
    }
}

void MainMenuState::ShowLobbyBrowser() {
    auto* context = m_App->GetUIManager()->GetRmlContext();
    if (!context)
        return;

    // Hide main menu
    if (m_Document) {
        m_Document->Hide();
    }

    // Load lobby browser
    m_LobbyBrowser = context->LoadDocument("assets/gui/lobby_browser.rml");
    if (!m_LobbyBrowser)
        return;

    // Populate lobby list
    auto* lobbyList = m_LobbyBrowser->GetElementById("lobby-list");
    if (lobbyList) {
        lobbyList->SetInnerRML("");

        if (Steam::FoundLobbies.empty()) {
            lobbyList->SetInnerRML("<p>No friend lobbies found.</p>");
        } else {
            std::string lobbyHtml;
            for (size_t i = 0; i < Steam::FoundLobbies.size(); i++) {
                lobbyHtml += "<div class='lobby-item'>";
                lobbyHtml += "<span>" + Steam::FoundLobbies[i].hostName +
                             "'s Lobby</span>";
                lobbyHtml += "<button id='join_lobby_" + std::to_string(i) +
                             "'>JOIN</button>";
                lobbyHtml += "</div>";
            }
            lobbyList->SetInnerRML(lobbyHtml);

            // Add event listeners to join buttons
            for (size_t i = 0; i < Steam::FoundLobbies.size(); i++) {
                std::string buttonId = "join_lobby_" + std::to_string(i);
                if (auto el = m_LobbyBrowser->GetElementById(buttonId))
                    el->AddEventListener(Rml::EventId::Click, this);
            }
        }
    }

    // Add back button listener
    if (auto el = m_LobbyBrowser->GetElementById("back"))
        el->AddEventListener(Rml::EventId::Click, this);

    m_LobbyBrowser->Show();
}
