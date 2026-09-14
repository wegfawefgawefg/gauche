#include "recoverable.hpp"
#include "../item_pattern.hpp"
#include "../combat/parry.hpp"
#include "../props/interaction.hpp"
#include "../world/ground_items.hpp"

#include <algorithm>

namespace {

void forget_hits(Game& game, Handle shot) {
    std::erase_if(game.flight_contacts, [shot](const FlightContact& hit) { return hit.projectile == shot; });
}

Item* reservation(Game& game, const Entity& shot, Handle handle) {
    Entity* owner = get_entity(game, shot.entity_b);
    if (owner != nullptr)
        for (Item& item : owner->inventory.slots)
            if (item.flight == handle) return &item;
    return nullptr;
}

void land(Game& game, int slot) {
    const Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot, shot.generation};
    Item item = shot.ground_item;
    item.flight = {};
    const Cell cell = nearby_ground_item_cell(game, shot.cell);
    if (Item* held = reservation(game, shot, handle)) *held = {};
    forget_hits(game, handle);
    remove_entity(game, handle);
    // CAPACITY: Converting a projectile frees a slot before its physical item is restored.
    if (Entity* loose = get_entity(game, spawn_entity(game, EntityKind::GroundItem, cell))) {
        loose->ground_item = item;
        loose->sprite = item_sprite(item);
    }
    emit_sound(game, item.kind == ItemKind::Boomerang ? SoundId::BoomerangLand : SoundId::RockImpact, cell);
}

bool catch_boomerang(Game& game, int slot) {
    const Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot, shot.generation};
    const Entity* owner = get_entity(game, shot.entity_b);
    if (owner == nullptr || owner->health <= 0 || owner->cell != shot.cell) return false;
    Item* held = reservation(game, shot, handle);
    if (held == nullptr) { land(game, slot); return true; }
    *held = shot.ground_item;
    held->flight = {};
    held->cooldown = item_pattern(*held).cooldown;
    const Cell cell = shot.cell;
    forget_hits(game, handle);
    remove_entity(game, handle);
    emit_sound(game, SoundId::BoomerangCatch, cell);
    return true;
}

void turn_back(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const Entity* owner = get_entity(game, shot.entity_b);
    if (owner == nullptr || owner->health <= 0) { land(game, slot); return; }
    shot.label_b = 1;
    shot.counter_a = shot.attack_interval * 2 + 8;
    shot.facing = cardinal_toward(shot.cell, owner->cell, shot.facing);
    shot.timer_b = 3;
    forget_hits(game, {slot, shot.generation});
    catch_boomerang(game, slot);
}

bool first_contact(Game& game, Handle shot, Handle victim) {
    for (const FlightContact& hit : game.flight_contacts)
        if (hit.projectile == shot && hit.victim == victim) return false;
    game.flight_contacts.push_back({shot, victim});
    return true;
}

} // namespace

// SLOTS: label_b outgoing/returning/parried (0/1/2); counter_a range, counter_b damage.
// entity_a current attacker; entity_b original reservation owner; timer_c hard flight deadline.
// Copied item owns the physical object; a returning weapon leaves a handle reservation.
bool launch_recoverable(Game& game, int owner_slot, Item& item, Cell direction) {
    Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    const Handle handle = spawn_entity(game, EntityKind::Projectile, owner.cell);
    Entity* shot = get_entity(game, handle);
    if (shot == nullptr) return false;
    const bool boomerang = item.kind == ItemKind::Boomerang;
    const ItemPattern pattern = item_pattern(item);
    shot->label_a = static_cast<int>(boomerang ? ProjectileKind::Boomerang : ProjectileKind::Rock);
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->counter_b = pattern.damage;
    shot->counter_c = pattern.piercing || has_artifact(owner, ArtifactKind::AllPiercing) ? 1 : 0;
    shot->point_a = owner.cell;
    shot->entity_a = shot->entity_b = {owner_slot, owner.generation};
    shot->facing = direction;
    shot->ground_item = item;
    shot->ground_item.count = 1;
    shot->ground_item.flight = {};
    shot->ground_item.cooldown = pattern.cooldown;
    shot->sprite = item_sprite(item);
    shot->timer_b = boomerang ? 3 : 4;
    shot->timer_a = (pattern.maximum * 3 + 10) * shot->timer_b;
    shot->timer_c = shot->timer_a;
    if (boomerang) item.flight = handle;
    return true;
}

void step_recoverable(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const bool boomerang = shot.label_a == static_cast<int>(ProjectileKind::Boomerang);
    const Entity* owner = get_entity(game, shot.entity_b);
    if (shot.timer_a == 0 || (boomerang && (owner == nullptr || owner->health <= 0))) {
        land(game, slot); return;
    }
    if (boomerang && shot.label_b == 1 && catch_boomerang(game, slot)) return;
    if (shot.timer_b > 0) return;
    if (boomerang && shot.label_b == 1) shot.facing = cardinal_toward(shot.cell, owner->cell, shot.facing);
    const Cell next = shot.cell + shot.facing;
    bool blocked = projectile_blocked(game, next);
    // ANCHORS: Turn before entering a crusher or a creature's closed shell.
    for (const Entity& actor : game.entities)
        if (actor.kind != EntityKind::None && actor.impassable && actor.hard_blocker && actor.cell == next)
            blocked = true;
    hit_prop(game, next, shot.counter_b, shot.cell);
    if (blocked) {
        if (!boomerang) hit_terrain(game, next, shot.cell, shot.counter_b, shot.ground_item.dig_power);
        if (boomerang && shot.label_b == 0) turn_back(game, slot);
        else land(game, slot);
        return;
    }
    shot.cell = next;
    --shot.counter_a;
    shot.timer_b = boomerang ? 3 : 4;
    if (boomerang && shot.label_b == 1 && catch_boomerang(game, slot)) return;
    for (int index = 0; index < max_entities; ++index) {
        const Entity& actor = game.entities[static_cast<std::size_t>(index)];
        if (index == slot || actor.health <= 0 || !actor.impassable || actor.cell != next ||
            (index == shot.entity_a.slot && actor.generation == shot.entity_a.generation)) continue;
        if (!first_contact(game, {slot, shot.generation}, {index, actor.generation})) continue;
        if (parry_ranged_hit(game, index, shot.facing)) {
            reflect_projectile(shot, actor, index);
            if (boomerang) shot.label_b = 2;
            forget_hits(game, {slot, shot.generation});
            return;
        }
        damage_entity(game, index, shot.counter_b, next - shot.facing);
        if ((boomerang && shot.label_b == 2) || (!boomerang && shot.counter_c == 0)) { land(game, slot); return; }
        emit_sound(game, boomerang ? SoundId::BoomerangHit : SoundId::RockImpact, next);
    }
    if (shot.counter_a == 0) {
        if (boomerang && shot.label_b == 0) turn_back(game, slot);
        else land(game, slot);
    }
}

// INTERLUDE: Return reserved tools before the reward screen freezes world simulation.
void finish_recoverables(Game& game) {
    for (int slot = 0; slot < max_entities; ++slot) {
        Entity& shot = game.entities[static_cast<std::size_t>(slot)];
        if (shot.kind != EntityKind::Projectile ||
            shot.label_a != static_cast<int>(ProjectileKind::Boomerang)) continue;
        const Entity* owner = get_entity(game, shot.entity_b);
        if (owner != nullptr && owner->health > 0 && shot.label_b != 2) {
            shot.cell = owner->cell;
            catch_boomerang(game, slot);
        } else land(game, slot);
    }
}
