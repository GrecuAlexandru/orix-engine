#pragma once

#include "game/network/NetworkPackets.hpp"
#include <steam/steam_api.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

class Steam {
  public:
    // === Initialization ===
    static bool Init();
    static void Update();
    static void Shutdown();

    // === Lobby Management ===
    static void CreateLobby();
    static void FindLobbies();
    static void JoinLobby(CSteamID lobbyId);
    static CSteamID GetCurrentLobbyID();
    static std::string GetUserName(CSteamID userSteamID);

    // === Networking ===
    static void SendPosition(glm::vec3 pos, glm::vec3 direction);
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
    static void OnLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure);
    static void OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure);
    STEAM_CALLBACK(Steam, OnLobbyChatUpdate, LobbyChatUpdate_t);

    // State
    inline static SteamAPICall_t m_LobbyCreateCall = k_uAPICallInvalid;
    inline static SteamAPICall_t m_LobbyMatchListCall = k_uAPICallInvalid;
    inline static SteamAPICall_t m_LobbyEnterCall = k_uAPICallInvalid;
    inline static CSteamID m_CurrentLobbyID;

    // Static packet counter for tickrate calculation
    inline static int s_PacketsReceivedThisSecond = 0;
};