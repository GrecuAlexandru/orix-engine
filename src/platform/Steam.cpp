#include "platform/Steam.hpp"
#include "game/network/NetworkPackets.hpp"
#include <iostream>
#include "steam/steamnetworkingtypes.h"

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

void Steam::SendPosition(glm::vec3 pos, glm::vec3 direction) {
    if (!m_CurrentLobbyID.IsValid())
        return;

    PlayerPositionPacket packet;
    packet.steamID = SteamUser()->GetSteamID().ConvertToUint64();
    packet.x = pos.x;
    packet.y = pos.y;
    packet.z = pos.z;
    packet.dirX = direction.x;
    packet.dirY = direction.y;
    packet.dirZ = direction.z;

    int numMembers = SteamMatchmaking()->GetNumLobbyMembers(m_CurrentLobbyID);
    for (int i = 0; i < numMembers; i++) {
        CSteamID target =
            SteamMatchmaking()->GetLobbyMemberByIndex(m_CurrentLobbyID, i);
        if (target == SteamUser()->GetSteamID())
            continue; // Don't send to ourselves

        SteamNetworkingIdentity identity;
        identity.SetSteamID(target);

        SteamNetworkingMessages()->SendMessageToUser(
            identity,
            &packet,
            sizeof(packet),
            k_nSteamNetworkingSend_Unreliable,
            0);
    }
}

void Steam::ReceivePackets() {
    SteamNetworkingMessage_t* pIncomingMsg = nullptr;
    while (SteamNetworkingMessages()->ReceiveMessagesOnChannel(
               0, &pIncomingMsg, 1) > 0) {
        if (pIncomingMsg) {
            PacketType* type = (PacketType*)pIncomingMsg->m_pData;
            if (*type == PacketType::PlayerPosition) {
                PlayerPositionPacket* p =
                    (PlayerPositionPacket*)pIncomingMsg->m_pData;

                // Initialize currentPos if this is a new player
                if (RemotePlayers.find(p->steamID) == RemotePlayers.end()) {
                    RemotePlayers[p->steamID].currentPos =
                        glm::vec3(p->x, p->y, p->z);
                }

                // Update the TARGET position, not the current position
                RemotePlayers[p->steamID].targetPos =
                    glm::vec3(p->x, p->y, p->z);
                RemotePlayers[p->steamID].direction =
                    glm::vec3(p->dirX, p->dirY, p->dirZ);

                // Increment packet counter for tickrate calculation
                s_PacketsReceivedThisSecond++;
            }
            pIncomingMsg->Release();
        }
    }
}

int Steam::GetAndResetPacketCount() {
    int count = s_PacketsReceivedThisSecond;
    s_PacketsReceivedThisSecond = 0;
    return count;
}

void Steam::InterpolatePlayers(float deltaTime) {
    // 10.0f is the "Smoothing Factor". Higher = faster response, Lower =
    // smoother but laggier.
    float lerpFactor = 10.0f * deltaTime;

    for (auto& [id, data] : RemotePlayers) {
        // Move currentPos slightly toward targetPos
        data.currentPos = glm::mix(data.currentPos, data.targetPos, lerpFactor);
    }
}

int Steam::GetPing(uint64_t targetID) {
    SteamNetConnectionRealTimeStatus_t status;

    SteamNetworkingIdentity identity;
    identity.SetSteamID(CSteamID(targetID));

    SteamNetworkingMessages()->GetSessionConnectionInfo(
        identity, nullptr, &status);
    return status.m_nPing;
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