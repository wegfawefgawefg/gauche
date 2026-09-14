#include "thunder.hpp"
#include "../entities/bell_diver.hpp"
#include "projectile.hpp"
#include "../surfaces/interaction.hpp"
#include "../world/water.hpp"
#include "../props/interaction.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

bool conductive_actor(const Entity& actor) {
    return actor.health > 0 && !actor.hard_blocker && actor.kind != EntityKind::GlassEel &&
        (actor.move_interval > 0 || actor.kind == EntityKind::RootTurret || actor.kind == EntityKind::WaspNest);
}

bool wet_actor(const Game& game, const Entity& actor) {
    return diver_submerged(actor) || (surface_wet(game.stage.at_or_border(actor.cell)) &&
        (wading_actor(actor) || actor.kind == EntityKind::RootTurret || actor.kind == EntityKind::WaspNest));
}

void arc_event(Game& game, Cell from, Cell to, bool husk) {
    if (game.shot_count < static_cast<int>(game.shots.size()))
        game.shots[static_cast<std::size_t>(game.shot_count++)] = {from, to, true, false, husk, true};
    emit_sound(game, SoundId::ThunderCrack, to);
}

void discharge(Game& game, int slot, Cell impact, int first) {
    const Entity shot = game.entities[static_cast<std::size_t>(slot)];
    const ItemPattern pattern = item_pattern(shot.ground_item);
    // COMMIT: Capture all victims and cells before deaths, drops or burning alter cover.
    const ThunderChain chain = thunder_chain(game, first, pattern);
    remove_entity(game, {slot, shot.generation});
    if (chain.count == 0) {
        arc_event(game, shot.cell, impact, true);
        hit_prop(game, impact, pattern.damage, shot.point_a);
        ignite_surface(game, impact);
        return;
    }
    Cell previous = shot.cell;
    for (int index = 0; index < chain.count; ++index) {
        const auto at = static_cast<std::size_t>(index);
        const Cell cell = chain.cells[at];
        const int damage = (pattern.damage * (4 - index) + 3) / 4;
        arc_event(game, previous, cell, index == 0);
        if (get_entity(game, chain.targets[at]))
            damage_entity(game, chain.targets[at].slot, damage, previous, false);
        hit_prop(game, cell, damage, shot.point_a);
        ignite_surface(game, cell);
        previous = cell;
    }
}

} // namespace

bool thunder_arc_clear(const Game& game, Cell from, Cell to) {
    // CONDUCTION: Water and smoke pass arcs; walls, solid props and closed fixtures stop them.
    const int dx = to.x - from.x, dy = to.y - from.y;
    const int nx = std::abs(dx), ny = std::abs(dy);
    const Cell sx{dx > 0 ? 1 : -1, 0}, sy{0, dy > 0 ? 1 : -1};
    int ix = 0, iy = 0;
    Cell cell = from;
    while (ix < nx || iy < ny) {
        const int crossing = (1 + 2 * ix) * ny - (1 + 2 * iy) * nx;
        if (crossing == 0) {
            if (projectile_blocked(game, cell + sx) || projectile_blocked(game, cell + sy)) return false;
            cell = cell + sx + sy; ++ix; ++iy;
        } else if (crossing < 0) { cell = cell + sx; ++ix; }
        else { cell = cell + sy; ++iy; }
        if (projectile_blocked(game, cell)) return false;
    }
    return true;
}

ThunderChain thunder_chain(const Game& game, int first, ItemPattern pattern) {
    ThunderChain chain;
    if (first < 0 || first >= max_entities || !conductive_actor(game.entities[static_cast<std::size_t>(first)])) return chain;
    int next = first;
    while (chain.count < static_cast<int>(chain.targets.size())) {
        const Entity& actor = game.entities[static_cast<std::size_t>(next)];
        const auto at = static_cast<std::size_t>(chain.count++);
        chain.targets[at] = {next, actor.generation};
        chain.cells[at] = actor.cell;
        const int reach = pattern.blast_radius + (wet_actor(game, actor) ? 2 : 0);
        int best = reach + 1;
        next = -1;
        for (int slot = 0; slot < max_entities; ++slot) {
            const Entity& other = game.entities[static_cast<std::size_t>(slot)];
            const Handle handle{slot, other.generation};
            if (!conductive_actor(other) || std::find(chain.targets.begin(), chain.targets.end(), handle) != chain.targets.end()) continue;
            const int gap = distance(actor.cell, other.cell);
            if (gap >= best || !thunder_arc_clear(game, actor.cell, other.cell)) continue;
            next = slot; best = gap;
        }
        if (next < 0) break;
    }
    return chain;
}

// SLOTS: counter_a range left, attack_interval total range, timer_a life, timer_b flight beat.
// entity_a owner, point_a launch cell, ground_item modifier-bearing payload.
bool launch_thunder_acorn(Game& game, int owner, const Item& item, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(owner)];
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, user.cell));
    if (!shot) return false;
    shot->label_a = static_cast<int>(ProjectileKind::ThunderAcorn);
    shot->sprite = Sprite::ThunderAcornLit;
    shot->entity_a = {owner, user.generation};
    shot->point_a = user.cell;
    shot->facing = direction;
    shot->ground_item = item; shot->ground_item.count = 1;
    shot->counter_a = shot->attack_interval = item_pattern(item).maximum;
    shot->timer_b = 4;
    shot->timer_a = shot->counter_a * 4 + 4;
    shot->light = {2, 330, {164, 215, 255}};
    return true;
}

void step_thunder_acorn(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (shot.timer_a == 0 || shot.counter_a == 0) { discharge(game, slot, shot.cell, -1); return; }
    if (shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    if (projectile_blocked(game, next)) { discharge(game, slot, next, -1); return; }
    const int victim = entity_at(game, next, true);
    if (victim >= 0 && Handle{victim, game.entities[static_cast<std::size_t>(victim)].generation} != shot.entity_a) {
        discharge(game, slot, next, victim);
        return;
    }
    shot.cell = next;
    --shot.counter_a;
    shot.timer_b = 4;
    if (shot.counter_a == 0) discharge(game, slot, shot.cell, -1);
}
