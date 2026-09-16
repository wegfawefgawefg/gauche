#include "diver_route.hpp"
#include "attacks.hpp"
#include "../props/interaction.hpp"

#include <vector>

bool diver_pool(TileKind kind) {
    return kind == TileKind::Ice || kind == TileKind::IceHole ||
        kind == TileKind::Water || kind == TileKind::DeepRiver || kind == TileKind::ShallowWater;
}

bool diver_hole_open(const Game& game, Cell cell, int diver_slot) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind != TileKind::IceHole || prop_blocks(tile->prop)) return false;
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        if (slot != diver_slot && actor.kind != EntityKind::None && actor.cell == cell && actor.impassable) return false;
    }
    return true;
}

DiverRoute diver_route(const Game& game, int slot, Cell target) {
    const Cell origin = game.entities[static_cast<std::size_t>(slot)].cell;
    DiverRoute best;
    if (!game.stage.in_bounds(origin)) return best;
    std::vector<std::uint8_t> seen(game.stage.tiles.size(), 0);
    const auto index = [&game](Cell cell) { return static_cast<std::size_t>(cell.y * game.stage.width + cell.x); };
    // UNDER ICE: Living actors do not block a swimmer below them; closed fixtures do.
    for (const Entity& actor : game.entities)
        if (actor.kind != EntityKind::None && actor.hard_blocker && actor.impassable && game.stage.in_bounds(actor.cell))
            seen[index(actor.cell)] = 1;
    struct Node { Cell cell, first; int steps; };
    std::vector<Node> queue{{origin, origin, 0}};
    seen[index(origin)] = 1;
    int score = 10000;
    constexpr Cell sides[]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    // BUDGET: A local basin search, never a teleport between unrelated ponds.
    for (std::size_t next = 0; next < queue.size() && next < 256; ++next) {
        const Node node = queue[next];
        if (diver_hole_open(game, node.cell, slot)) {
            const int candidate = distance(node.cell, target) * 8 + node.steps +
                (clear_attack_sight(game, node.cell, target) ? 0 : 100);
            if (candidate < score) { score = candidate; best = {true, node.cell, node.first}; }
        }
        if (node.steps >= 12) continue;
        for (Cell side : sides) {
            const Cell cell = node.cell + side;
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || seen[index(cell)] || !diver_pool(tile->kind) || prop_blocks(tile->prop)) continue;
            seen[index(cell)] = 1;
            queue.push_back({cell, node.steps == 0 ? cell : node.first, node.steps + 1});
        }
    }
    return best;
}
