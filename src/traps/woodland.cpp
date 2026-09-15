#include "contact.hpp"
#include "woodland.hpp"
#include "../items/woodland_traps.hpp"
#include "../combat/shove.hpp"
#include "../projectiles/projectile.hpp"
#include "../props/interaction.hpp"
#include "../world/water.hpp"

#include <algorithm>

namespace {

void release_snare(Game& game, int slot, bool broken) {
    Entity& trap = game.entities[static_cast<std::size_t>(slot)];
    if (Entity* captive = get_entity(game, trap.entity_a))
        if (captive->vitals.root_kind == RootKind::Rope && captive->vitals.rooted <= trap.timer_a)
            captive->vitals.rooted = 0;
    const Cell cell = trap.cell;
    if (broken) remove_entity(game, {slot, trap.generation});
    else {
        // RECOVERY: Reuse the fixture slot, even when the entity pool is full.
        trap.kind = EntityKind::GroundItem;
        trap.health = trap.max_health = 1;
        trap.sprite = Sprite::RopeSnare;
        trap.ground_item.cooldown = item_pattern(trap.ground_item).cooldown;
        trap.entity_a = {};
        trap.timer_a = trap.label_a = 0;
    }
    emit_sound(game, SoundId::SnareRelease, cell);
}

void burst_acorn(Game& game, int slot) {
    const Entity& trap = game.entities[static_cast<std::size_t>(slot)];
    const Cell center = trap.cell;
    const ItemPattern pattern = item_pattern(trap.ground_item);
    // CHAIN: Remove the shell before applying damage; another mine may burst back at us.
    remove_entity(game, {slot, trap.generation});
    emit_sound(game, SoundId::AcornBurst, center);
    const int standing = entity_at(game, center, true);
    if (standing >= 0) damage_entity(game, standing, pattern.damage, center, false);
    hit_prop(game, center, pattern.damage, center);
    for (Cell direction : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
        Cell cell = center;
        for (int reach = 1; reach <= pattern.blast_radius; ++reach) {
            cell = cell + direction;
            const bool blocked = projectile_blocked(game, cell);
            hit_prop(game, cell, pattern.damage, center);
            hit_ground_traps(game, cell, pattern.damage, center);
            if (blocked) break;
            const int target = entity_at(game, cell, true);
            if (target >= 0) {
                damage_entity(game, target, pattern.damage, center);
                break;
            }
        }
        if (game.shot_count < static_cast<int>(game.shots.size()))
            game.shots[static_cast<std::size_t>(game.shot_count++)] = {center, cell, false, false, false};
    }
}

void spring_actor(Game& game, int trap_slot, int actor_slot) {
    const Entity& trap = game.entities[static_cast<std::size_t>(trap_slot)];
    const Cell origin = trap.cell, direction = trap.facing;
    const Handle actor_handle{actor_slot, game.entities[static_cast<std::size_t>(actor_slot)].generation};
    remove_entity(game, {trap_slot, trap.generation});
    emit_sound(game, SoundId::SpringLaunch, origin);
    for (int step = 0; step < 2; ++step) {
        const Entity* actor = get_entity(game, actor_handle);
        if (actor == nullptr || actor->health <= 0 || actor->hard_blocker) break;
        const Cell before = actor->cell;
        if (!shove_actor(game, actor_slot, direction, origin)) break;
        actor = get_entity(game, actor_handle);
        // ANOTHER TRAP: A nested launch owns its travel; do not add this spring's remainder.
        if (actor == nullptr || actor->health <= 0 || actor->cell != before + direction) break;
    }
}

void trigger(Game& game, int trap_slot, int actor_slot) {
    Entity& trap = game.entities[static_cast<std::size_t>(trap_slot)];
    Entity& actor = game.entities[static_cast<std::size_t>(actor_slot)];
    if (trap.timer_a > 0 || trap.label_a != 0 || trap.health <= 0 ||
        !wading_actor(actor) || actor.hard_blocker) return;
    switch (trap.ground_item.kind) {
    case ItemKind::RopeSnare:
        if (!apply_root(actor, 180)) return;
        trap.entity_a = {actor_slot, actor.generation};
        trap.label_a = 1; trap.timer_a = actor.vitals.rooted;
        trap.sprite = Sprite::SnareTight;
        emit_sound(game, SoundId::SnareCatch, trap.cell);
        break;
    case ItemKind::SpringTrap: spring_actor(game, trap_slot, actor_slot); break;
    case ItemKind::AcornMine: burst_acorn(game, trap_slot); break;
    default: break;
    }
}

} // namespace

bool woodland_trap(const Entity& trap) {
    return trap.kind == EntityKind::Trap && forest_trap_item(trap.ground_item.kind) != nullptr;
}

void step_woodland_trap(Game& game, int slot) {
    Entity& trap = game.entities[static_cast<std::size_t>(slot)];
    const Tile* tile = game.stage.at(trap.cell);
    if (tile != nullptr && tile->surface.fire_ticks > 0 && game.tick % 30 == 0)
        trap.health = std::max(0, trap.health - 5);
    if (trap.health <= 0) {
        if (trap.ground_item.kind == ItemKind::AcornMine) burst_acorn(game, slot);
        else if (trap.ground_item.kind == ItemKind::RopeSnare) release_snare(game, slot, true);
        else { const Cell cell = trap.cell; remove_entity(game, {slot, trap.generation}); emit_sound(game, SoundId::SpringBreak, cell); }
        return;
    }
    if (trap.label_a == 1) {
        const Entity* captive = get_entity(game, trap.entity_a);
        if (trap.timer_a == 0 || captive == nullptr || captive->health <= 0 ||
            captive->cell != trap.cell || captive->vitals.rooted == 0) release_snare(game, slot, false);
        return;
    }
    if (trap.timer_a == 0) trap.sprite = trap.ground_item.kind == ItemKind::RopeSnare ?
        Sprite::SnareSet : trap.ground_item.kind == ItemKind::SpringTrap ? Sprite::SpringReady : Sprite::AcornReady;
    const int actor = entity_at(game, trap.cell, true);
    if (actor >= 0) trigger(game, slot, actor);
}

void enter_woodland_traps(Game& game, int actor_slot) {
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(actor_slot)];
        const Entity& trap = game.entities[static_cast<std::size_t>(slot)];
        if (actor.health <= 0) return;
        if (woodland_trap(trap) && trap.cell == actor.cell && trap.birth_tick != game.tick)
            trigger(game, slot, actor_slot);
    }
}

bool hit_woodland_traps(Game& game, Cell cell, int damage, Cell source) {
    bool hit = false;
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& trap = game.entities[static_cast<std::size_t>(slot)];
        if (!woodland_trap(trap) || trap.cell != cell || damage <= 0) continue;
        damage_entity(game, slot, damage, source, false);
        if (trap.health <= 0) step_woodland_trap(game, slot);
        hit = true;
    }
    return hit;
}
