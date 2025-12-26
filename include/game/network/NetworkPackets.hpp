#pragma once
#include <steam/steam_api.h>
#include <glm/glm.hpp>

enum class PacketType : uint8_t {
    PlayerPosition = 0
};

#pragma pack(push, 1) // Ensure no padding between variables
struct PlayerPositionPacket {
    PacketType type = PacketType::PlayerPosition;
    uint64_t steamID;
    float x, y, z;
    float dirX, dirY, dirZ; // Front vector of the player
};
#pragma pack(pop)