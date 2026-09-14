#pragma once
#include "../game.hpp"

inline bool wooden_terrain(const Tile& tile) {
    return tile.material == TileMaterial::Timber || tile.material == TileMaterial::Tree;
}

inline Tile wood_tile(TileMaterial material) {
    Tile tile{TileKind::Wall, static_cast<std::uint16_t>(material == TileMaterial::Tree ? 90 : 60), 0};
    tile.max_hp = tile.hp;
    tile.break_rule = BreakRule::Damageable;
    tile.required_dig_power = 0;
    tile.material = material;
    return tile;
}
