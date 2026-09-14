#include "route.hpp"
#include "terrain_material.hpp"

#include <cstdlib>

namespace {

bool gated_room(const FloorPlan& plan, Cell cell) {
    for (int index : {plan.exit_room, plan.secret_room}) {
        if (index < 0) continue;
        const RoomPlan& room = plan.rooms[static_cast<std::size_t>(index)];
        if (std::abs(cell.x - room.center.x) <= room.half_width + 1 &&
            std::abs(cell.y - room.center.y) <= room.half_height + 1) return true;
    }
    return false;
}

bool exposed(const Stage& stage, Cell cell) {
    for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}})
        if (walkable(stage.at_or_border(cell + side).kind)) return true;
    return false;
}

void wooded_edges(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    const bool timber = room.role == RoomRole::Workshop;
    const bool forest = room.role == RoomRole::Thicket || room.role == RoomRole::Orchard ||
        room.role == RoomRole::Den || room.role == RoomRole::Clearing || room.role == RoomRole::Brook;
    if (!timber && !forest) return;
    const bool vertical = random_u32(game) % 2 == 0;
    // PATCHES: Long timber partitions and grouped tree edges, never per-pixel noise.
    for (int y = -room.half_height - 1; y <= room.half_height + 1; ++y)
        for (int x = -room.half_width - 1; x <= room.half_width + 1; ++x) {
            const Cell cell = room.center + Cell{x, y};
            Tile* tile = game.stage.at(cell);
            if (tile == nullptr || tile->kind != TileKind::Wall ||
                tile->break_rule == BreakRule::Unbreakable || gated_room(plan, cell) ||
                plan.protected_cell(cell) || !exposed(game.stage, cell)) continue;
            if (timber ? (vertical ? std::abs(x) < 3 : std::abs(y) < 3) :
                (vertical ? x < 2 : y < 2)) continue;
            *tile = wood_tile(timber ? TileMaterial::Timber : TileMaterial::Tree);
        }
}

void tree_clump(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    if (room.role != RoomRole::Orchard && room.role != RoomRole::Thicket &&
        room.role != RoomRole::Clearing) return;
    const int attempts = room.role == RoomRole::Thicket ? 5 : 3;
    for (int attempt = 0; attempt < attempts; ++attempt) {
        const Cell anchor = room.center + Cell{
            static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_width * 2 + 1)) - room.half_width,
            static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_height * 2 + 1)) - room.half_height};
        for (Cell offset : {Cell{0, 0}, {1, 0}}) {
            const Cell cell = anchor + offset;
            Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !walkable(*tile) || plan.protected_cell(cell) || gated_room(plan, cell) ||
                std::abs(cell.x - room.center.x) > room.half_width ||
                std::abs(cell.y - room.center.y) > room.half_height ||
                tile->kind == TileKind::ShallowWater || tile->kind == TileKind::Spring) continue;
            *tile = wood_tile(TileMaterial::Tree);
        }
    }
}

} // namespace

void place_forest_terrain(Game& game, const FloorPlan& plan) {
    if (game.run.floor > 4) return;
    for (const RoomPlan& room : plan.rooms) {
        wooded_edges(game, plan, room);
        tree_clump(game, plan, room);
    }
}
