#include "platform/Steam.hpp"

Steam* Steam::s_Instance = nullptr;
SteamAPICall_t Steam::m_LobbyCreateCall = k_uAPICallInvalid;
SteamAPICall_t Steam::m_LobbyMatchListCall = k_uAPICallInvalid;
SteamAPICall_t Steam::m_LobbyEnterCall = k_uAPICallInvalid;
CSteamID Steam::m_CurrentLobbyID;

Steam::Steam() {
    // Constructor - STEAM_CALLBACK macro handles initialization automatically
}

Steam::~Steam() {
    // Destructor
}

Steam* Steam::GetInstance() {
    if (!s_Instance) {
        s_Instance = new Steam();
    }
    return s_Instance;
}

bool Steam::Init() {
    if (SteamAPI_RestartAppIfNecessary(480))
        return false;
    if (!SteamAPI_Init())
        return false;

    // Create singleton instance to register callbacks
    GetInstance();

    return true;
}

void Steam::Update() {
    SteamAPI_RunCallbacks();

    // Check if lobby creation is complete
    if (m_LobbyCreateCall != k_uAPICallInvalid) {
        bool bFailed = false;
        if (SteamUtils()->IsAPICallCompleted(m_LobbyCreateCall, &bFailed)) {
            LobbyCreated_t callback;
            bool bIOFailure = false;
            if (SteamUtils()->GetAPICallResult(m_LobbyCreateCall,
                                               &callback,
                                               sizeof(callback),
                                               callback.k_iCallback,
                                               &bIOFailure)) {
                OnLobbyCreated(&callback, bIOFailure);
            }
            m_LobbyCreateCall = k_uAPICallInvalid;
        }
    }

    // Check if lobby match list is complete
    if (m_LobbyMatchListCall != k_uAPICallInvalid) {
        bool bFailed = false;
        if (SteamUtils()->IsAPICallCompleted(m_LobbyMatchListCall, &bFailed)) {
            LobbyMatchList_t callback;
            bool bIOFailure = false;
            if (SteamUtils()->GetAPICallResult(m_LobbyMatchListCall,
                                               &callback,
                                               sizeof(callback),
                                               callback.k_iCallback,
                                               &bIOFailure)) {
                OnLobbyMatchList(&callback, bIOFailure);
            }
            m_LobbyMatchListCall = k_uAPICallInvalid;
        }
    }

    // Check if lobby enter is complete
    if (m_LobbyEnterCall != k_uAPICallInvalid) {
        bool bFailed = false;
        if (SteamUtils()->IsAPICallCompleted(m_LobbyEnterCall, &bFailed)) {
            LobbyEnter_t callback;
            bool bIOFailure = false;
            if (SteamUtils()->GetAPICallResult(m_LobbyEnterCall,
                                               &callback,
                                               sizeof(callback),
                                               callback.k_iCallback,
                                               &bIOFailure)) {
                OnLobbyEntered(&callback, bIOFailure);
            }
            m_LobbyEnterCall = k_uAPICallInvalid;
        }
    }
}

void Steam::CreateLobby() {
    std::cout << "Steam: Requesting Lobby Creation..." << std::endl;
    m_LobbyCreateCall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 4);
}

void Steam::OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
    if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
        std::cerr << "Steam: Failed to create lobby." << std::endl;
        return;
    }
    m_CurrentLobbyID = pCallback->m_ulSteamIDLobby;
    std::cout << "Steam: Lobby Created! ID: " << pCallback->m_ulSteamIDLobby
              << std::endl;
}

void Steam::FindLobbies() {
    std::cout << "Steam: Searching for lobbies..." << std::endl;

    // Add filters (search worldwide)
    SteamMatchmaking()->AddRequestLobbyListDistanceFilter(
        k_ELobbyDistanceFilterWorldwide);

    m_LobbyMatchListCall = SteamMatchmaking()->RequestLobbyList();
}

void Steam::OnLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure) {
    if (bIOFailure) {
        std::cerr << "Steam: Failed to get lobby list." << std::endl;
        return;
    }

    std::cout << "Steam: Found " << pCallback->m_nLobbiesMatching << " lobbies."
              << std::endl;

    if (pCallback->m_nLobbiesMatching > 0) {
        // Join the first lobby found
        CSteamID lobbyID = SteamMatchmaking()->GetLobbyByIndex(0);
        JoinLobby(lobbyID);
    } else {
        std::cout << "Steam: No lobbies found." << std::endl;
    }
}

void Steam::JoinLobby(CSteamID lobbyId) {
    std::cout << "Steam: Joining Lobby " << lobbyId.ConvertToUint64() << "..."
              << std::endl;
    m_LobbyEnterCall = SteamMatchmaking()->JoinLobby(lobbyId);
}

void Steam::OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure) {
    if (bIOFailure) {
        std::cerr << "Steam: Failed to join lobby (IO Failure)." << std::endl;
        return;
    }

    if (pCallback->m_EChatRoomEnterResponse ==
        k_EChatRoomEnterResponseSuccess) {
        m_CurrentLobbyID = pCallback->m_ulSteamIDLobby;
        std::cout << "Steam: Successfully joined the lobby!" << std::endl;
    } else {
        std::cerr << "Steam: Failed to join lobby. Error code: "
                  << pCallback->m_EChatRoomEnterResponse << std::endl;
    }
}

void Steam::OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback) {
    // Get the ID of the person who caused the change
    CSteamID userChangedID = pCallback->m_ulSteamIDUserChanged;
    std::string userName = GetUserName(userChangedID);

    // Check what actually happened
    if (pCallback->m_rgfChatMemberStateChange &
        k_EChatMemberStateChangeEntered) {
        std::cout << "[Network] " << userName << " joined the game."
                  << std::endl;
    } else if (pCallback->m_rgfChatMemberStateChange &
               k_EChatMemberStateChangeLeft) {
        std::cout << "[Network] " << userName << " left the game." << std::endl;
    } else if (pCallback->m_rgfChatMemberStateChange &
               k_EChatMemberStateChangeDisconnected) {
        std::cout << "[Network] " << userName << " lost connection."
                  << std::endl;
    }
}

CSteamID Steam::GetCurrentLobbyID() {
    return m_CurrentLobbyID;
}

std::string Steam::GetUserName(CSteamID userSteamID) {
    // SteamFriends allows us to get the "Persona Name" of any user we can see
    return SteamFriends()->GetFriendPersonaName(userSteamID);
}

void Steam::Shutdown() {
    if (s_Instance) {
        delete s_Instance;
        s_Instance = nullptr;
    }
    SteamAPI_Shutdown();
}