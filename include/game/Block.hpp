#pragma once
#include <cstdint>

enum class BlockType : uint8_t {
    Air = 0,
    Grass,
    Dirt,
    Stone,
};

struct Block {
    BlockType type;

    Block() : type(BlockType::Air) {}
    Block(BlockType t) : type(t) {}

    bool IsActive() const {
        return type != BlockType::Air;
    }
};