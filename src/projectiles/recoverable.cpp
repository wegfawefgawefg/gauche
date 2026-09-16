#include "recoverable.hpp"
#include "flight_contacts.hpp"
#include "../world/chasm.hpp"
#include "../items/lunch_tin.hpp"
#include "../items/coal.hpp"
#include "../entities/boiler_tank.hpp"
#include "../surfaces/temperature.hpp"
#include "../item_pattern.hpp"
#include "../combat/parry.hpp"
#include "../props/interaction.hpp"

#include <algorithm>

namespace {

Item* reservation(Game& game, const Entity& shot, Handle handle) {
    Entity* owner = get_entity(game, shot.entity_b);
    if (owner != nullptr)
        for (Item& item : owner->inventory.slots)
            if (item.flight == handle) return &item;
    return nullptr;
}

void land(Game& game, int slot, bool hot_impact = false) {
    const Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot, shot.generation};
    Item item = shot.ground_item;
    item.flight = {};
    const bool melted = item.kind == ItemKind::IceNeedle && (hot_impact || hot_cell(game, shot.cell));
    const Cell impact=shot.cell;
    // Physical throws land where their flight ended, including hazards and
    // occupied item piles. Safe loot placement must not teleport them ashore.
    const Cell cell = shot.cell;
    const bool burned=coal_lava_contact(game,item,cell);
    if (Item* held = reservation(game, shot, handle)) *held = {};
    remove_entity(game, handle);
    if (burned) return;
    if (melted) { emit_sound(game, SoundId::IceMelt, cell); return; }
    // CAPACITY: Converting a projectile frees a slot before its physical item is restored.
    if (Entity* loose = get_entity(game, spawn_entity(game, EntityKind::GroundItem, cell))) {
        loose->ground_item = item;
        loose->sprite = item_sprite(item);
        if (chasm_contact(game,static_cast<int>(loose-game.entities.data()))) return;
    }
    if (item.kind==ItemKind::LunchTin) {land_lunch_tin(game,impact);return;}
    emit_sound(game, item.kind == ItemKind::GlowSlag ? SoundId::GlowLand : item.kind == ItemKind::Boomerang ? SoundId::BoomerangLand : item.kind == ItemKind::IceNeedle ? SoundId::IceNeedleHit : item.kind == ItemKind::CoalLump ? SoundId::CoalLand : SoundId::RockImpact, cell);
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
    forget_flight_contacts(game, {slot, shot.generation});
    catch_boomerang(game, slot);
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
    const bool needle=shot.ground_item.kind==ItemKind::IceNeedle;
    bool blocked = projectile_blocked(game, next,false,needle);
    // ANCHORS: Turn before entering a crusher or a creature's closed shell.
    for (const Entity& actor : game.entities)
        if (actor.kind != EntityKind::None && actor.impassable && actor.hard_blocker && actor.cell == next)
            blocked = true;
    const int tank = boiler_at(game,next);
    if (tank >= 0) damage_entity(game,tank,shot.counter_b,shot.cell);
    if (!needle || !prop_shoot_through(game.stage.at_or_border(next).prop))
        hit_prop(game, next, shot.counter_b, shot.cell);
    if (blocked) {
        if (!boomerang) hit_terrain(game, next, shot.cell, shot.counter_b, shot.ground_item.dig_power);
        if (boomerang && shot.label_b == 0) turn_back(game, slot);
        else land(game, slot, hot_cell(game, next));
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
        if (!first_flight_contact(game, {slot, shot.generation}, {index, actor.generation})) continue;
        if (parry_ranged_hit(game, index, shot.facing)) {
            reflect_projectile(shot, actor, index);
            if (boomerang) shot.label_b = 2;
            forget_flight_contacts(game, {slot, shot.generation});
            return;
        }
        const int health = actor.health;
        damage_entity(game, index, shot.counter_b, next - shot.facing, true, shot.entity_a);
        if (shot.ground_item.kind == ItemKind::IceNeedle && actor.health < health && !hot_cell(game, next))
            apply_chill(game.entities[static_cast<std::size_t>(index)], 60);
        if ((boomerang && shot.label_b == 2) || (!boomerang && shot.counter_c == 0)) { land(game, slot); return; }
        emit_sound(game, boomerang ? SoundId::BoomerangHit : shot.ground_item.kind == ItemKind::IceNeedle ? SoundId::IceNeedleHit : shot.ground_item.kind == ItemKind::CoalLump ? SoundId::CoalLand : SoundId::RockImpact, next);
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
