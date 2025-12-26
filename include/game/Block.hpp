#pragma once
#include <cstdint>

enum class BlockType : uint8_t {
    Air = 0,
    Grass,
    Dirt,
    Stone,
};

struct Block {
    BlockType id;

    Block() : id(BlockType::Air) {}
    Block(BlockType type) : id(type) {}

    bool IsActive() const {
        return id != BlockType::Air;
    }
};