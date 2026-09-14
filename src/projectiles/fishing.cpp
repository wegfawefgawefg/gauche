#include "fishing.hpp"
#include "../item_pattern.hpp"
#include "../world/floating_items.hpp"

namespace {

bool fishing_hook(const Entity& actor) {
    return actor.kind == EntityKind::Projectile && actor.label_a == static_cast<int>(ProjectileKind::FishingHook);
}

void finish(Game& game, int slot, bool snapped) {
    const Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    emit_sound(game, snapped ? SoundId::FishingSnap : SoundId::FishingFinish, hook.cell);
    remove_entity(game, {slot, hook.generation});
}

bool blocked(const Game& game, const Entity& hook, Cell cell) {
    if (projectile_blocked(game, cell)) return true;
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        if (actor.kind == EntityKind::None || !actor.impassable || actor.cell != cell) continue;
        if (slot == hook.entity_a.slot && actor.generation == hook.entity_a.generation) continue;
        return true;
    }
    return false;
}

bool clear_line(const Game& game, const Entity& hook) {
    const int along = distance(hook.point_a, hook.cell);
    if (along > hook.attack_interval ||
        hook.cell != hook.point_a + Cell{hook.facing.x*along, hook.facing.y*along}) return false;
    for (int step = 1; step <= along; ++step)
        if (blocked(game, hook, hook.point_a + Cell{hook.facing.x*step, hook.facing.y*step})) return false;
    return true;
}

int loose_at(const Game& game, Cell cell, int except = -1) {
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        if (slot != except && actor.kind == EntityKind::GroundItem && actor.cell == cell &&
            actor.ground_item.kind != ItemKind::None && actor.ground_item.count > 0) return slot;
    }
    return -1;
}

bool already_caught(const Game& game, Handle cargo) {
    for (const Entity& other : game.entities)
        if (fishing_hook(other) && other.label_b == FishingCargo && other.entity_b == cargo) return true;
    return false;
}

void return_empty(Game& game, Entity& hook) {
    hook.label_b = FishingEmpty;
    hook.counter_a = 0;
    hook.timer_b = fishing_beat;
    emit_sound(game, SoundId::FishingTap, hook.cell);
}

void cast_step(Game& game, Entity& hook) {
    const Cell next = hook.cell + hook.facing;
    if (blocked(game, hook, next)) { return_empty(game, hook); return; }
    hook.cell = next;
    hook.timer_b = fishing_beat;
    const int slot = loose_at(game, next);
    if (slot >= 0) {
        Entity& cargo = game.entities[static_cast<std::size_t>(slot)];
        const Handle handle{slot, cargo.generation};
        if (already_caught(game, handle)) { return_empty(game, hook); return; }
        stop_item_float(game, cargo);
        hook.entity_b = handle;
        hook.point_b = cargo.cell;
        hook.label_b = FishingCargo;
        hook.counter_a = 0;
        emit_sound(game, SoundId::FishingCatch, next);
        return;
    }
    if (--hook.counter_a == 0) return_empty(game, hook);
}

void reel_step(Game& game, int slot) {
    Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    if (hook.cell == hook.point_a) { finish(game, slot, false); return; }
    const Cell next = hook.cell - hook.facing;
    if (blocked(game, hook, next)) { finish(game, slot, true); return; }
    if (hook.label_b == FishingCargo) {
        Entity* cargo = get_entity(game, hook.entity_b);
        // OWNERSHIP: Pickup, displacement or another float releases the real item.
        if (!cargo || cargo->kind != EntityKind::GroundItem || cargo->ground_item.count <= 0 || cargo->cell != hook.point_b || floating_item(*cargo)) {
            finish(game, slot, true); return;
        }
        if (loose_at(game, next, hook.entity_b.slot) >= 0) { finish(game, slot, true); return; }
        cargo->cell = hook.point_b = next;
    }
    hook.cell = next;
    hook.timer_b = fishing_beat;
    if (hook.cell == hook.point_a) { finish(game, slot, false); return; }
    emit_sound(game, SoundId::FishingReel, hook.cell);
}

} // namespace

// SLOTS: label_b outbound/empty/cargo; entity_a owner; entity_b actual loose item.
// point_a fixed cast origin; point_b expected cargo cell; counter_a outbound cells.
// timer_a hard lifetime; timer_b beat; attack_interval reach. No item is copied into cargo.
bool launch_fishing_hook(Game& game, int owner_slot, const Item& item, Cell direction) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    if (distance({}, direction) != 1 || item.uses <= 0) return false;
    const Handle handle{owner_slot, owner.generation};
    for (const Entity& other : game.entities)
        if (fishing_hook(other) && other.entity_a == handle) return false;
    Entity* hook = get_entity(game, spawn_entity(game, EntityKind::Projectile, owner.cell));
    if (!hook) return false;
    hook->label_a = static_cast<int>(ProjectileKind::FishingHook);
    hook->label_b = FishingOut;
    hook->sprite = Sprite::FishingHook;
    hook->entity_a = handle;
    hook->point_a = owner.cell;
    hook->facing = direction;
    hook->counter_a = hook->attack_interval = item_pattern(item).maximum;
    hook->timer_a = hook->counter_a*fishing_beat*2 + 24;
    hook->timer_b = fishing_beat;
    hook->ground_item = item;
    return true;
}

void step_fishing_hook(Game& game, int slot) {
    Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    const Entity* owner = get_entity(game, hook.entity_a);
    if (!owner || owner->health <= 0 || owner->cell != hook.point_a || hook.timer_a == 0 ||
        !clear_line(game, hook)) { finish(game, slot, true); return; }
    if (hook.label_b == FishingCargo) {
        const Entity* cargo = get_entity(game, hook.entity_b);
        if (!cargo || cargo->kind != EntityKind::GroundItem || cargo->ground_item.count <= 0 || cargo->cell != hook.point_b || floating_item(*cargo)) {
            finish(game, slot, true); return;
        }
    }
    if (hook.timer_b > 0) return;
    if (hook.label_b == FishingOut) cast_step(game, hook);
    else reel_step(game, slot);
}
