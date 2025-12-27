#pragma once

#include "game/network/NetworkPackets.hpp"
#include <steam/steam_api.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

struct LobbyInfo {
    CSteamID id;
    std::string hostName;
};

class Steam {
  public:
    // === Initialization ===
    static bool Init();
    static void Update();
    static void Shutdown();

    // === Lobby Management ===
    static void CreateLobby();
    static void FindLobbies();
    static void FindFriendLobbies();
    static void JoinLobby(CSteamID lobbyId);
    static CSteamID GetCurrentLobbyID();
    static std::string GetUserName(CSteamID userSteamID);
    inline static std::vector<LobbyInfo> FoundLobbies;

    // === Networking ===
    static void SendPosition(glm::vec3 pos, float yaw, float pitch);
    static void ReceivePackets();
    static void InterpolatePlayers(float deltaTime);
    static int GetPing(uint64_t targetID);
    static int GetAndResetPacketCount();

    // === Remote Player Data ===
    inline static std::map<uint64_t, RemotePlayerData> RemotePlayers;

  private:
    // Singleton for callbacks
    Steam();
    ~Steam();
    static Steam* GetInstance();
    inline static Steam* s_Instance = nullptr;

    // Lobby callbacks
    static void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    void OnLobbyMatchList(LobbyMatchList_t* pCallback,
                          bool bIOFailure); // Non-static for CCallResult
    static void OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure);
    STEAM_CALLBACK(Steam, OnLobbyChatUpdate, LobbyChatUpdate_t);
    CCallResult<Steam, LobbyMatchList_t> m_LobbyMatchListCallResult;

    // State
    inline static SteamAPICall_t m_LobbyCreateCall = k_uAPICallInvalid;
    inline static SteamAPICall_t m_LobbyMatchListCall = k_uAPICallInvalid;
    inline static SteamAPICall_t m_LobbyEnterCall = k_uAPICallInvalid;
    inline static CSteamID m_CurrentLobbyID;

    // Static packet counter for tickrate calculation
    inline static int s_PacketsReceivedThisSecond = 0;
};