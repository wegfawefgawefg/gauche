#pragma once

#include "../game.hpp"

// PALETTE: These assets carry their own color; no full-screen blue multiplier.
inline Sprite ice_tile_sprite(const Tile& tile, Cell cell, std::uint64_t tick) {
    switch (tile.kind) {
    case TileKind::Empty: case TileKind::Grass:
        return (cell.x * 17 + cell.y * 31) % 7 == 0 ? Sprite::IceFloorB : Sprite::IceFloorA;
    case TileKind::Wall: return Sprite::IceWall;
    case TileKind::Ruin: return Sprite::IceRuin;
    case TileKind::IceHole: return Sprite::IceHole;
    case TileKind::Snow: return Sprite::Snow;
    case TileKind::Ice: return Sprite::ReservoirIce;
    case TileKind::ShallowWater:
        return tick / 45 % 2 == 0 ? Sprite::ColdShallowA : Sprite::ColdShallowB;
    case TileKind::Water:
        return (tick / 60 + tile.water_phase) % 2 == 0 ? Sprite::ColdWaterA : Sprite::ColdWaterB;
    default: return Sprite::Count;
    }
}
