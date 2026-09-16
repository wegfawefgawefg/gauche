#pragma once

#include "../game.hpp"

#include <cstdlib>

Handle place_ground_item(Game& game, Cell cell, ItemKind kind, int count = 1);

inline Cell nearby_ground_item_cell(const Game& game, Cell origin) {
    // A drop over a river stays there for raft pickup or sinking, never shore rescue.
    if (game.stage.at_or_border(origin).kind==TileKind::DeepRiver) return origin;
    // PLACEMENT: Search nearest tiles in a fixed order so every peer agrees.
    for (int distance = 0; distance <= 8; ++distance)
        for (int dy = -distance; dy <= distance; ++dy) {
            const int dx = distance - std::abs(dy);
            for (int sign : {-1, 1}) {
                if (dx == 0 && sign == 1) continue;
                const Cell candidate{origin.x + sign * dx, origin.y + dy};
                const Tile* tile = game.stage.at(candidate);
                if (tile == nullptr || !walkable(*tile) ||
                    tile->kind == TileKind::Lava) continue;
                bool occupied = false;
                for (const Entity& entity : game.entities)
                    if (entity.kind != EntityKind::None && entity.cell == candidate &&
                        (entity.kind == EntityKind::GroundItem ||
                         (candidate != origin && entity.impassable))) {
                        occupied = true;
                        break;
                    }
                if (!occupied) return candidate;
            }
        }
    // What if a room is completely packed? Keep the item on its original tile.
    return origin;
}
