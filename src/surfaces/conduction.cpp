#include "conduction.hpp"
#include "interaction.hpp"
#include "../world/water.hpp"
#include "../entities/bell_diver.hpp"
#include "../projectiles/projectile.hpp"

#include <algorithm>

namespace {

bool shockable(const Entity& actor) {
    return actor.health > 0 && !actor.hard_blocker && actor.kind != EntityKind::GlassEel &&
        (actor.move_interval > 0 || actor.kind == EntityKind::RootTurret || actor.kind == EntityKind::WaspNest);
}

} // namespace

bool conductive_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && surface_wet(*tile) && !projectile_blocked(game, cell);
}

bool water_shock_target(const Game& game, const Entity& actor) {
    return shockable(actor) && conductive_cell(game, actor.cell) &&
        (diver_submerged(actor) || wading_actor(actor) ||
         actor.kind == EntityKind::RootTurret || actor.kind == EntityKind::WaspNest);
}

WetWave wet_wave(const Game& game, Cell origin, int reach) {
    WetWave wave;
    if (!game.stage.at(origin) || projectile_blocked(game, origin)) return wave;
    wave.nodes[0] = {origin, 0, 0};
    wave.count = 1;
    if (!conductive_cell(game, origin)) return wave;
    reach = std::clamp(reach, 0, 6);
    // PATH: Dry banks, frozen water and solid cover break the circuit. No diagonal leaks.
    for (int next = 0; next < wave.count; ++next) {
        const WetNode node = wave.nodes[static_cast<std::size_t>(next)];
        if (node.steps >= reach) continue;
        for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
            const Cell cell = node.cell + side;
            if (!conductive_cell(game, cell)) continue;
            bool seen = false;
            for (int i = 0; i < wave.count; ++i)
                if (wave.nodes[static_cast<std::size_t>(i)].cell == cell) { seen = true; break; }
            if (!seen && wave.count < static_cast<int>(wave.nodes.size()))
                wave.nodes[static_cast<std::size_t>(wave.count++)] = {cell, next, node.steps + 1};
        }
    }
    return wave;
}

void discharge_water(Game& game, Cell origin, Cell attacker, int damage, int reach) {
    reach = std::clamp(reach, 0, 6);
    const WetWave wave = wet_wave(game, origin, reach);
    struct Victim { Handle handle; int steps; };
    std::vector<Victim> victims;
    // SNAPSHOT: A death, dropped item or reflected effect cannot extend this pulse's circuit.
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        if (!shockable(actor)) continue;
        for (int i = 0; i < wave.count; ++i) {
            const WetNode node = wave.nodes[static_cast<std::size_t>(i)];
            if (actor.cell != node.cell || (i > 0 && !water_shock_target(game, actor))) continue;
            victims.push_back({{slot, actor.generation}, node.steps});
            break;
        }
    }
    for (int i = 0; i < wave.count; ++i) {
        const WetNode node = wave.nodes[static_cast<std::size_t>(i)];
        const Cell previous = i == 0 ? attacker : wave.nodes[static_cast<std::size_t>(node.parent)].cell;
        if (game.shot_count < static_cast<int>(game.shots.size()))
            game.shots[static_cast<std::size_t>(game.shot_count++)] = {previous, node.cell, i == 0, false, false, true};
    }
    for (const Victim& victim : victims) {
        const int power = (damage * (reach + 1 - victim.steps) + reach) / (reach + 1);
        if (get_entity(game, victim.handle)) damage_entity(game, victim.handle.slot, power, attacker, false);
    }
}
