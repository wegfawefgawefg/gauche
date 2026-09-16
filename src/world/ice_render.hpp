#pragma once

#include "../game.hpp"

inline Sprite ice_wall_sprite(const Tile& tile,Cell cell) {
    const bool alternate=(cell.x/2+cell.y/3)%2!=0;
    if (tile.material!=TileMaterial::Ice) return alternate ? Sprite::IceRockB : Sprite::IceRockA;
    if (tile.max_hp<=60) return alternate ? Sprite::IceClearB : Sprite::IceClearA;
    return alternate ? Sprite::IceCloudB : Sprite::IceCloudA;
}

// PALETTE: These assets carry their own color; no full-screen blue multiplier.
inline Sprite ice_tile_sprite(const Tile& tile, Cell cell, std::uint64_t tick) {
    switch (tile.kind) {
    case TileKind::Empty: case TileKind::Grass:
        return (cell.x * 17 + cell.y * 31) % 7 == 0 ? Sprite::IceFloorB : Sprite::IceFloorA;
    case TileKind::Wall: return ice_wall_sprite(tile,cell);
    case TileKind::Ruin: return Sprite::IceRuin;
    case TileKind::IceHole: return Sprite::IceHole;
    case TileKind::Snow: return (cell.x/3+cell.y/2)%2==0 ? Sprite::PackedSnowA : Sprite::PackedSnowB;
    case TileKind::Ice: return Sprite::ReservoirIce;
    case TileKind::ShallowWater:
        return tick / 45 % 2 == 0 ? Sprite::ColdShallowA : Sprite::ColdShallowB;
    case TileKind::DeepRiver:
    case TileKind::Water:
        return (tick / 60 + tile.water_phase) % 2 == 0 ? Sprite::ColdWaterA : Sprite::ColdWaterB;
    default: return Sprite::Count;
    }
}
