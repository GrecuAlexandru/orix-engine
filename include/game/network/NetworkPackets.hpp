#pragma once
#include <steam/steam_api.h>
#include <glm/glm.hpp>

enum class PacketType : uint8_t {
    PlayerPosition = 0
};

#pragma pack(push, 1)
struct PlayerPositionPacket {
    PacketType type = PacketType::PlayerPosition;
    uint64_t steamID;
    float x, y, z;
    float dirX, dirY, dirZ;
};
#pragma pack(pop)

// Remote player data for interpolation
struct RemotePlayerData {
    glm::vec3 currentPos; // Smoothly interpolated position
    glm::vec3 targetPos;  // Latest received position
    glm::vec3 direction;  // Player facing direction
};