#include "behavior.hpp"

#include <optional>
#include <vector>

namespace {

std::optional<Cell> route_step(const Game& game, int slot, Cell target) {
    const Cell origin = game.entities[static_cast<std::size_t>(slot)].cell;
    if (!game.stage.in_bounds(target) || origin == target) return std::nullopt;
    std::vector<std::uint8_t> visited(game.stage.tiles.size(), 0);
    const auto index = [&game](Cell cell) {
        return static_cast<std::size_t>(cell.y * game.stage.width + cell.x);
    };
    // OCCUPANCY: Build once per search rather than scanning every actor at every tile.
    for (const Entity& blocker : game.entities)
        if (blocker.kind != EntityKind::None && blocker.impassable &&
            blocker.cell != target && blocker.cell != origin && game.stage.in_bounds(blocker.cell))
            visited[index(blocker.cell)] = 1;
    struct Node { Cell cell, first; };
    std::vector<Node> queue{{origin, origin}};
    visited[index(origin)] = 1;
    const Cell delta = target - origin;
    const Cell horizontal{delta.x >= 0 ? 1 : -1, 0};
    const Cell vertical{0, delta.y >= 0 ? 1 : -1};
    const Cell sides[]{horizontal, vertical, Cell{-horizontal.x, 0}, Cell{0, -vertical.y}};
    for (std::size_t next = 0; next < queue.size() && next < 4096; ++next) {
        const Node node = queue[next];
        for (Cell side : sides) {
            const Cell cell = node.cell + side;
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || !walkable(*tile) || visited[index(cell)] != 0) continue;
            const Cell first = node.cell == origin ? cell : node.first;
            if (cell == target) return first;
            visited[index(cell)] = 1;
            queue.push_back({cell, first});
        }
    }
    return std::nullopt;
}

} // namespace

void pursue(Game& game, int slot, Cell target) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.move_wait > 0 || actor.cell == target) return;
    if (const auto next = route_step(game, slot, target)) move_entity(game, slot, *next);
    else wander(game, slot);
}
