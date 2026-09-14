#include "hearing.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <array>

namespace {

bool curious(EntityKind kind) {
    switch (kind) {
    case EntityKind::Wolf: case EntityKind::Bear: case EntityKind::Boar:
    case EntityKind::Dog: case EntityKind::Zombie: case EntityKind::ZombieStack:
    case EntityKind::IceMason: case EntityKind::ForagerGoblin: case EntityKind::CrateMimic: return true;
    default: return false;
    }
}

bool skittish(EntityKind kind) {
    switch (kind) {
    case EntityKind::Chicken: case EntityKind::Bunny: case EntityKind::Bat:
    case EntityKind::FrostBat: case EntityKind::CarrionCrow: case EntityKind::Owl:
    case EntityKind::Woodpecker: case EntityKind::Mosquito: case EntityKind::Wasp:
    case EntityKind::LanternMoth: case EntityKind::SporeToad: return true;
    default: return false;
    }
}

} // namespace

std::vector<Cell> audible_cells(const Game& game, Cell origin, int radius) {
    radius = std::clamp(radius, 0, 12);
    constexpr int side = 25;
    std::array<bool, side * side> blocked{};
    const auto index = [origin](Cell cell) {
        return static_cast<std::size_t>((cell.y - origin.y + 12) * side + cell.x - origin.x + 12);
    };
    // ACOUSTICS: Bounded cardinal flood. Bodies do not seal sound; walls/closed doors do.
    for (int y = -radius; y <= radius; ++y)
        for (int x = -radius; x <= radius; ++x) {
            const Cell cell = origin + Cell{x, y};
            const Tile* tile = game.stage.at(cell);
            blocked[index(cell)] = tile == nullptr || tile->kind == TileKind::Wall || prop_blocks(tile->prop);
        }
    for (const Entity& fixture : game.entities)
        if (fixture.impassable && distance(fixture.cell, origin) <= radius &&
            (fixture.kind == EntityKind::Door || fixture.kind == EntityKind::EncounterGate))
            blocked[index(fixture.cell)] = true;
    std::vector<Cell> cells{origin};
    std::vector<int> steps{0};
    blocked[index(origin)] = true;
    for (std::size_t next = 0; next < cells.size(); ++next) {
        if (steps[next] >= radius) continue;
        for (Cell direction : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
            const Cell cell = cells[next] + direction;
            if (distance(cell, origin) > radius || blocked[index(cell)]) continue;
            blocked[index(cell)] = true;
            cells.push_back(cell);
            steps.push_back(steps[next] + 1);
        }
    }
    return cells;
}

void make_noise(Game& game, Cell origin, int radius, int startle_radius) {
    const auto heard = audible_cells(game, origin, radius);
    const auto startled = startle_radius > 0 ? audible_cells(game, origin, startle_radius) : std::vector<Cell>{};
    for (Entity& actor : game.entities) {
        if (actor.health <= 0 ||
            std::find(heard.begin(), heard.end(), actor.cell) == heard.end()) continue;
        actor.sleep_ticks = 0;
        if (skittish(actor.kind) && std::find(startled.begin(), startled.end(), actor.cell) != startled.end()) {
            actor.point_c = origin; actor.label_c = StartleNoise; actor.timer_c = 150;
            apply_stun(actor, 30);
        } else if (curious(actor.kind) && actor.label_c != StartleNoise) {
            actor.point_c = origin; actor.label_c = InvestigateNoise; actor.timer_c = 300;
        }
    }
}

bool step_hearing(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    if (actor.timer_c <= 0 || (actor.label_c != InvestigateNoise && actor.label_c != StartleNoise)) return false;
    if (actor.label_c == StartleNoise) { flee(game, slot, actor.point_c); return true; }
    // THREATS: Adjacent visible players and actual hits take priority over a distant bell.
    const int threat = nearest_player(game, actor.cell, 1);
    if (threat >= 0 && clear_sight(game, actor.cell, game.entities[static_cast<std::size_t>(threat)].cell)) {
        actor.timer_c = actor.label_c = 0;
        return false;
    }
    if (actor.cell == actor.point_c || (distance(actor.cell, actor.point_c) == 1 && entity_at(game, actor.point_c, true) >= 0)) {
        actor.timer_c = actor.label_c = 0;
        return false;
    }
    if (actor.move_wait > 0) return true;
    if (const auto next = next_route_cell(game, slot, actor.point_c, 512)) willing_step(game, slot, *next);
    else { actor.timer_c = actor.label_c = 0; return false; }
    return true;
}
