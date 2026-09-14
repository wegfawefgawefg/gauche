#include "../game.hpp"

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
        if (entity.kind == EntityKind::Key || entity.kind == EntityKind::Switch)
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
        if (cell == game.run.exit) return true;
        for (Cell direction : directions) pending.push_back({cell + direction, has_key});
    }
    return false;
}
