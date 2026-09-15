#pragma once

#include "../game.hpp"

// SURFACES: Native color replaces the old orange multiplier on Forest tiles.
// Art does not change digging permissions, lava contact or wet/cold interactions.
inline Sprite industrial_tile_sprite(const Tile& tile, Cell cell, std::uint64_t tick) {
    const unsigned int variation=static_cast<unsigned int>(cell.x*17+cell.y*31);
    switch (tile.kind) {
    case TileKind::Empty: case TileKind::Grass:
        return variation%11==0 ? Sprite::IndustrialFloorB : Sprite::IndustrialFloorA;
    case TileKind::Wall: return Sprite::IndustrialWall;
    case TileKind::Ruin: return Sprite::IndustrialRuin;
    case TileKind::Lava:
        if (variation%7!=0) return Sprite::IndustrialLava;
        return (tick/60+variation)%2==0 ? Sprite::IndustrialLavaA : Sprite::IndustrialLavaB;
    default: return Sprite::Count;
    }
}
