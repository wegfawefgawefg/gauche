#include "frozen_pilgrim.hpp"
#include "steam_leech.hpp"
#include "../props/candle.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>
#include <array>
#include <vector>

std::optional<Cell> pilgrim_warm_step(const Game& game, int slot) {
    constexpr int radius = 9, side = radius * 2 + 1;
    constexpr Cell offsets[]{{0,0},{1,0},{-1,0},{0,1},{0,-1}};
    const Cell origin = game.entities[static_cast<std::size_t>(slot)].cell;
    const auto inside = [origin](Cell cell) { return distance(origin, cell) <= radius; };
    const auto index = [origin](Cell cell) {
        return static_cast<std::size_t>((cell.y - origin.y + radius)*side + cell.x - origin.x + radius);
    };
    std::array<bool, side*side> warm{}, flame{}, blocked{}, seen{};
    const auto drains = leech_drain_cells(game);
    const auto drained = [&drains](Cell cell) {
        return std::find(drains.begin(), drains.end(), cell) != drains.end();
    };
    const auto mark_flame = [&](Cell cell) {
        if (drained(cell)) return;
        if (inside(cell)) flame[index(cell)] = true;
        for (Cell offset : offsets)
            if (inside(cell + offset)) warm[index(cell + offset)] = true;
    };
    // HEAT: Only actual flames and capsule patches count; colored lamps are not stoves.
    for (int y = origin.y-radius-1; y <= origin.y+radius+1; ++y)
        for (int x = origin.x-radius-1; x <= origin.x+radius+1; ++x) {
            const Cell cell{x,y};
            const Tile* tile = game.stage.at(cell);
            if (!tile) continue;
            if (candle_lit(tile->prop) || tile->surface.fire_ticks > 0 || tile->kind == TileKind::Lava) mark_flame(cell);
            if (!inside(cell)) continue;
            blocked[index(cell)] = !walkable(*tile);
            if (tile->surface.warmth_ticks > 0 && !drained(cell)) warm[index(cell)] = true;
        }
    for (const Entity& actor : game.entities) {
        if (actor.kind == EntityKind::None) continue;
        if (distance(origin, actor.cell) <= radius+1 && entity_has_flame(actor)) mark_flame(actor.cell);
        if (inside(actor.cell) && actor.impassable && actor.cell != origin) blocked[index(actor.cell)] = true;
    }
    // ROUTE: One bounded flood finds reachable warmth, including around bends.
    // Never walk directly into an exposed flame just to stand near it.
    struct Node { Cell cell, first; };
    std::vector<Node> queue{{origin, origin}};
    seen[index(origin)] = true;
    for (std::size_t next = 0; next < queue.size(); ++next) {
        const Node node = queue[next];
        const Cell cell = node.cell;
        if (warm[index(cell)] && !flame[index(cell)]) return node.first;
        for (Cell offset : {Cell{1,0},{0,1},{-1,0},{0,-1}}) {
            const Cell neighbor = cell + offset;
            if (!inside(neighbor) || !game.stage.in_bounds(neighbor)) continue;
            const auto at = index(neighbor);
            if (seen[at] || blocked[at] || flame[at]) continue;
            seen[at] = true;
            queue.push_back({neighbor, cell == origin ? neighbor : node.first});
        }
    }
    return std::nullopt;
}
