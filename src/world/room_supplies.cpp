#include "room_supplies.hpp"
#include "../entities/dispatch.hpp"
#include "../entities/boiler_porter.hpp"
#include "../entities/seal_thief.hpp"
#include "../surfaces/interaction.hpp"

std::vector<Cell> room_spaces(const Game& game, const RoomPlan& room, EntityKind kind) {
    const int width = room.half_width * 2 + 1;
    std::vector<bool> seen(static_cast<std::size_t>(width * (room.half_height * 2 + 1)), false);
    std::vector<Cell> queue{room.center}, choices;
    constexpr Cell sides[]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (std::size_t next = 0; next < queue.size(); ++next) {
        const Cell cell = queue[next];
        const int x = cell.x - room.center.x + room.half_width;
        const int y = cell.y - room.center.y + room.half_height;
        if (x < 0 || x >= width || y < 0 || y > room.half_height * 2) continue;
        const auto index = static_cast<std::size_t>(y * width + x);
        if (seen[index]) continue;
        seen[index] = true;
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || tile->kind == TileKind::Lava) continue;
        if (distance(cell, game.run.spawn) >= 4 && entity_at(game, cell, false) < 0 &&
            tile->kind != TileKind::Spring && (kind != EntityKind::RimeSkater || tile->kind == TileKind::Ice) &&
            (kind != EntityKind::BellDiver || tile->kind == TileKind::IceHole) &&
            (kind != EntityKind::GlassEel || surface_wet(*tile)) &&
            (kind != EntityKind::SnowBurrower || tile->kind == TileKind::Snow) &&
            (kind != EntityKind::SealThief || seal_bank(game,cell)))
            choices.push_back(cell);
        for (Cell side : sides) queue.push_back(cell + side);
    }
    return choices;
}

std::optional<Cell> room_space(Game& game, const RoomPlan& room, EntityKind kind) {
    const auto choices=room_spaces(game,room,kind);
    // ISLANDS: Required supplies never roll onto a bank isolated by water or lava.
    if (choices.empty()) return std::nullopt;
    return choices[random_u32(game) % choices.size()];
}

Handle spawn_room_enemy(Game& game, const RoomPlan& room, EntityKind kind, int cost, RoomSupplies& budget) {
    PlacementCount* count=budget.report ? &budget.report->enemies[static_cast<std::size_t>(kind)] : nullptr;
    if (cost > budget.threat) {if (count) ++count->budget_blocked;return {};}
    if (count) ++count->attempted;
    if (const auto cell = room_space(game, room, kind)) {
        const Handle spawned = kind == EntityKind::BurrowWorm ?
            spawn_burrow_worm(game, *cell) : kind == EntityKind::BoilerPorter ?
            spawn_boiler_porter(game,*cell) : spawn_entity(game, kind, *cell);
        if (get_entity(game, spawned) != nullptr) {budget.threat -= cost;if (count) ++count->placed;}
        else if (count) ++count->rejected;
        return spawned;
    }
    if (count) ++count->rejected;
    return {};
}
