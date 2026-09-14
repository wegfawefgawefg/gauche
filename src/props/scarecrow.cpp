#include "scarecrow.hpp"
#include "../entities/attacks.hpp"

#include <algorithm>

namespace {
constexpr int maximum_radius = 5;
bool live_ward(const Prop& prop) {
    return prop.kind == PropKind::Scarecrow && prop.hp > 0 && !prop.broken;
}
int ward_radius(const Prop& prop) { return std::clamp(static_cast<int>(prop.variant), 4, maximum_radius); }
}

// WILLING STEPS: A ward cannot undo an already committed dive, drill or theft.
bool scarecrow_shy(const Entity& actor) {
    switch (actor.kind) {
    case EntityKind::Chicken: case EntityKind::Bunny: return true;
    case EntityKind::CarrionCrow: return actor.label_a != 1;
    case EntityKind::Owl: case EntityKind::Woodpecker:
        return actor.label_a != 1 && actor.label_a != 2;
    default: return false;
    }
}

bool scarecrow_covers(const Game& game, Cell ward, Cell cell, int radius) {
    return distance(ward, cell) <= radius && clear_sight(game, ward, cell);
}

int scarecrow_pressure(const Game& game, Cell cell) {
    int pressure = 0;
    for (int y = -maximum_radius; y <= maximum_radius; ++y)
        for (int x = -maximum_radius; x <= maximum_radius; ++x) {
            const Cell ward = cell + Cell{x, y};
            const Tile* tile = game.stage.at(ward);
            if (tile == nullptr || !live_ward(tile->prop)) continue;
            const int radius = ward_radius(tile->prop);
            if (scarecrow_covers(game, ward, cell, radius))
                pressure = std::max(pressure, radius + 1 - distance(ward, cell));
        }
    return pressure;
}

bool scarecrow_allows_step(const Game& game, const Entity& actor, Cell destination) {
    if (!scarecrow_shy(actor)) return true;
    return scarecrow_pressure(game, destination) <= scarecrow_pressure(game, actor.cell);
}

// RETREAT: Leave an overlapping ward by the least frightening free neighbor.
// If penned in, hesitate instead of overwriting flock, prey or perch memory.
bool step_scarecrow_fear(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (!scarecrow_shy(actor)) return false;
    const int pressure = scarecrow_pressure(game, actor.cell);
    if (pressure == 0) return false;
    if (actor.move_wait > 0) return true;
    Cell best = actor.cell;
    int lowest = pressure;
    const Cell sides[]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    const int offset = static_cast<int>((game.tick / 30 + static_cast<std::uint64_t>(slot)) % 4);
    for (int i = 0; i < 4; ++i) {
        const Cell cell = actor.cell + sides[(i + offset) % 4];
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, true) >= 0) continue;
        const int next = scarecrow_pressure(game, cell);
        if (next < lowest || (best == actor.cell && next == lowest)) { best = cell; lowest = next; }
    }
    if (best != actor.cell) move_entity(game, slot, best);
    else actor.move_wait = std::max(1, actor.move_interval / 2);
    return true;
}

// ROUTES: Bake wards once into a bird's search mask, avoiding a scan per BFS node.
void mark_scarecrow_wards(const Game& game, const Entity& actor, std::span<std::uint8_t> blocked) {
    if (!scarecrow_shy(actor)) return;
    for (int y = 0; y < game.stage.height; ++y)
        for (int x = 0; x < game.stage.width; ++x) {
            const Cell ward{x, y};
            const Prop& prop = game.stage.at(ward)->prop;
            if (!live_ward(prop)) continue;
            const int radius = ward_radius(prop);
            for (int dy = -radius; dy <= radius; ++dy)
                for (int dx = -radius; dx <= radius; ++dx) {
                    const Cell cell = ward + Cell{dx, dy};
                    if (game.stage.in_bounds(cell) && scarecrow_covers(game, ward, cell, radius))
                        blocked[static_cast<std::size_t>(cell.y * game.stage.width + cell.x)] = 1;
                }
        }
}
