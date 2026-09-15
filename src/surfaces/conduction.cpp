#include "conduction.hpp"
#include "interaction.hpp"
#include "../world/water.hpp"
#include "../entities/bell_diver.hpp"
#include "../projectiles/projectile.hpp"
#include "../props/circuits.hpp"

#include <algorithm>

namespace {

bool shockable(const Entity& actor) {
    return actor.health > 0 && !actor.hard_blocker && actor.kind != EntityKind::GlassEel &&
        (actor.move_interval > 0 || actor.kind == EntityKind::RootTurret || actor.kind == EntityKind::WaspNest);
}

void arc(Game& game, Cell from, Cell to, bool impact) {
    if (game.shot_count < static_cast<int>(game.shots.size()))
        game.shots[static_cast<std::size_t>(game.shot_count++)] = {from,to,impact,false,false,true};
}

void find_ground(const Game& game, WetWave& wave) {
    // ORDER: Nearest connected node first, then a stable cardinal order. No arc through cover.
    for (int i=0;i<wave.count;++i) {
        const Cell cell=wave.nodes[static_cast<std::size_t>(i)].cell;
        for (Cell offset : {Cell{0,0},{1,0},{-1,0},{0,1},{0,-1}}) {
            const Cell candidate=cell+offset;
            if (!grounding_ready(game.stage.at_or_border(candidate).prop) || projectile_blocked(game,candidate)) continue;
            wave.ground=candidate; wave.ground_node=i; return;
        }
    }
}

} // namespace

bool conductive_water(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && surface_wet(*tile) && !projectile_blocked(game, cell);
}

bool conductive_cell(const Game& game, Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && (surface_wet(*tile) || circuit_prop(tile->prop)) && !projectile_blocked(game,cell);
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
    if (!conductive_cell(game, origin)) { find_ground(game,wave); return wave; }
    reach = std::clamp(reach, 0, 6);
    // PATH: Dry banks and ice need actual wire. Solid cover breaks all cardinal circuits.
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
    find_ground(game,wave);
    return wave;
}

ConductedShock trace_conducted_shock(const Game& game,Cell origin,int reach) {
    ConductedShock shock;
    shock.reach=std::clamp(reach,0,6);
    shock.wave=wet_wave(game,origin,shock.reach);
    const WetWave& wave=shock.wave;
    if (wave.ground_node>=0) return shock;
    // SNAPSHOT: A death, dropped item or reflected effect cannot extend this pulse's circuit.
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        if (!shockable(actor)) continue;
        for (int i = 0; i < wave.count; ++i) {
            const WetNode node = wave.nodes[static_cast<std::size_t>(i)];
            if (actor.cell != node.cell || (i > 0 && !water_shock_target(game, actor))) continue;
            shock.victims.push_back({{slot, actor.generation}, node.steps});
            break;
        }
    }
    return shock;
}

void apply_conducted_shock(Game& game,const ConductedShock& shock,Cell attacker,int damage) {
    const WetWave& wave=shock.wave;
    if (wave.count==0) return;
    const Cell origin=wave.nodes[0].cell;
    const int reach=shock.reach;
    if (wave.ground_node>=0) {
        // GROUND: One ready spike sinks the complete pulse before any actors take damage.
        // Present only the path into that sink, not damaging branches that never happened.
        int next=wave.ground_node;
        arc(game,wave.nodes[static_cast<std::size_t>(next)].cell,wave.ground,true);
        while (next>0) {
            const WetNode node=wave.nodes[static_cast<std::size_t>(next)];
            arc(game,wave.nodes[static_cast<std::size_t>(node.parent)].cell,node.cell,false);
            next=node.parent;
        }
        arc(game,attacker,origin,false);
        absorb_grounded_shock(game,wave.ground);
        return;
    }
    for (int i = 0; i < wave.count; ++i) {
        const WetNode node = wave.nodes[static_cast<std::size_t>(i)];
        const Cell previous = i == 0 ? attacker : wave.nodes[static_cast<std::size_t>(node.parent)].cell;
        arc(game,previous,node.cell,i==0);
    }
    for (const ShockVictim& victim : shock.victims) {
        const int power = (damage * (reach + 1 - victim.steps) + reach) / (reach + 1);
        Entity* actor=get_entity(game,victim.handle);
        if (!actor) continue;
        // The origin is direct electrode contact, even on wet ground. Only
        // later circuit nodes enter through the floor; boots do not cut wire
        // or protect anyone farther downstream, and cannot block the origin.
        if (victim.steps>0 && actor->vitals.floor_insulation>0) {
            emit_sound(game,SoundId::InsulatorArc,actor->cell);continue;
        }
        damage_entity(game,victim.handle.slot,power,attacker,false);
    }
}

void discharge_water(Game& game,Cell origin,Cell attacker,int damage,int reach) {
    apply_conducted_shock(game,trace_conducted_shock(game,origin,reach),attacker,damage);
}
