#include "route.hpp"

#include <array>
#include <deque>
#include <utility>
#include <vector>

bool floor_reachable(const Game& game) {
    const int count = game.stage.width * game.stage.height;
    if (count <= 0) return false;
    std::vector<bool> visited(static_cast<std::size_t>(count * 2), false);
    std::deque<std::pair<Cell, bool>> pending;
    pending.push_back({game.run.spawn, false});
    constexpr std::array<Cell, 4> directions{{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    Cell key{-1, -1};
    Cell door{-1, -1};
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::Key || entity.kind == EntityKind::Switch ||
            entity.kind == EntityKind::Encounter)
            key = entity.cell;
        if (entity.kind == EntityKind::Door) door = entity.cell;
    }
    while (!pending.empty()) {
        auto [cell, has_key] = pending.front();
        pending.pop_front();
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile)) continue;
        has_key |= cell == key;
        if (cell == door && !has_key) continue;
        const int index = (cell.y * game.stage.width + cell.x) * 2 + (has_key ? 1 : 0);
        if (visited[static_cast<std::size_t>(index)]) continue;
        visited[static_cast<std::size_t>(index)] = true;
        if (cell == game.run.exit && (has_key || game.run.has_key)) return true;
        for (Cell direction : directions) pending.push_back({cell + direction, has_key});
    }
    return false;
}


bool floor_lock_required(const Game& game) {
    Cell door{-1, -1}, objective{-1, -1};
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::Door) door = entity.cell;
        if (entity.kind == EntityKind::Key || entity.kind == EntityKind::Switch) objective = entity.cell;
    }
    if (!game.stage.in_bounds(door) || !game.stage.in_bounds(objective)) return false;
    // WALKING ONLY: Ignore mobile actors and refuse the locked tile. Digging is
    // intentionally outside this check, as it is one of the player's escape tools.
    std::vector<bool> seen(game.stage.tiles.size(), false);
    std::vector<Cell> queue{game.run.spawn};
    bool reached_objective = false;
    constexpr Cell directions[]{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (std::size_t next = 0; next < queue.size(); ++next) {
        const Cell cell = queue[next];
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || cell == door) continue;
        const auto index = static_cast<std::size_t>(cell.y * game.stage.width + cell.x);
        if (seen[index]) continue;
        seen[index] = true;
        if (cell == game.run.exit) return false;
        if (cell == objective) reached_objective = true;
        for (Cell side : directions) queue.push_back(cell + side);
    }
    return reached_objective;
}
