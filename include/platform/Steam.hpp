#pragma once

#include <steam/steam_api.h>
#include <iostream>

class Steam {
  public:
    static bool Init();
    static void Update();
    static void Shutdown();

    static void CreateLobby();
    static void FindLobbies();
    static void JoinLobby(CSteamID lobbyId);

    static std::string GetUserName(CSteamID userSteamID);
    static CSteamID GetCurrentLobbyID();

  private:
    Steam(); // Private constructor for singleton
    ~Steam();

    static Steam* GetInstance();
    static Steam* s_Instance;

    void OnLobbyCreatedInternal(LobbyCreated_t* pCallback, bool bIOFailure);
    void OnLobbyMatchListInternal(LobbyMatchList_t* pCallback, bool bIOFailure);
    void OnLobbyEnteredInternal(LobbyEnter_t* pCallback, bool bIOFailure);

    static void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    static void OnLobbyMatchList(LobbyMatchList_t* pCallback, bool bIOFailure);
    static void OnLobbyEntered(LobbyEnter_t* pCallback, bool bIOFailure);

    STEAM_CALLBACK(Steam, OnLobbyChatUpdate, LobbyChatUpdate_t);

    static SteamAPICall_t m_LobbyCreateCall;
    static SteamAPICall_t m_LobbyMatchListCall;
    static SteamAPICall_t m_LobbyEnterCall;
    static CSteamID m_CurrentLobbyID;
};