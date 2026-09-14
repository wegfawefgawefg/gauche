#include "hook.hpp"
#include "../items/displacement.hpp"
#include "../combat/shove.hpp"

namespace {

void release(Game& game, int slot) {
    const Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    emit_sound(game, SoundId::HookRelease, hook.cell);
    remove_entity(game, {slot, hook.generation});
}

bool straight_tether(const Game& game, Cell from, Cell to, Cell direction, int range) {
    const Cell delta = to - from;
    const int along = delta.x * direction.x + delta.y * direction.y;
    if (along < 1 || along > range || delta.x * direction.y != delta.y * direction.x) return false;
    for (int step = 1; step < along; ++step)
        if (projectile_blocked(game, from + Cell{direction.x * step, direction.y * step})) return false;
    return true;
}

void reel(Game& game, int slot, Entity& owner) {
    Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    Entity* target = get_entity(game, hook.entity_b);
    if (hook.label_b == 1) {
        if (target == nullptr || !movable_by_tool(*target)) { release(game, slot); return; }
        hook.cell = target->cell;
    } else if (hook.entity_b.slot >= 0) {
        if (target == nullptr || !target->impassable || movable_by_tool(*target) ||
            target->cell != hook.point_b) { release(game, slot); return; }
    } else {
        const Tile* anchor = game.stage.at(hook.point_b);
        if (anchor == nullptr || (anchor->kind != TileKind::Wall && !prop_blocks(anchor->prop))) { release(game, slot); return; }
    }
    if (!straight_tether(game, owner.cell, hook.cell, hook.facing, hook.attack_interval) ||
        distance(owner.cell, hook.cell) <= 1 || hook.counter_a <= 0) { release(game, slot); return; }
    if (hook.timer_b > 0) return;
    const int moving = hook.label_b == 1 ? hook.entity_b.slot : hook.entity_a.slot;
    const Cell direction = hook.label_b == 1 ? Cell{-hook.facing.x, -hook.facing.y} : hook.facing;
    if (hook.label_b == 2) {
        const Cell destination = owner.cell + direction;
        const Tile* ground = game.stage.at(destination);
        if (ground == nullptr || !walkable(*ground) || entity_at(game, destination, true) >= 0) {
            release(game, slot); return;
        }
    }
    if (!shove_actor(game, moving, direction, owner.cell)) { release(game, slot); return; }
    if (hook.label_b == 1 && target != nullptr) hook.cell = target->cell;
    --hook.counter_a;
    hook.timer_b = 4;
    emit_sound(game, SoundId::HookReel, hook.cell);
}

} // namespace

// SLOTS: label_b outbound/target/anchor; entity_a owner; entity_b caught actor;
// point_b anchor cell; counter_a flight range/reel steps; timer_a lifetime; timer_b beat.
bool launch_hook(Game& game, int owner_slot, const Item& item, Cell direction) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    // One tether per user, including when swapping to another hook instance.
    for (const Entity& shot : game.entities)
        if (shot.kind == EntityKind::Projectile && shot.label_a == static_cast<int>(ProjectileKind::Hook) &&
            shot.entity_a.slot == owner_slot && shot.entity_a.generation == owner.generation) return false;
    Entity* hook = get_entity(game, spawn_entity(game, EntityKind::Projectile, owner.cell));
    if (hook == nullptr) return false;
    hook->label_a = static_cast<int>(ProjectileKind::Hook);
    hook->sprite = Sprite::HookHead;
    hook->entity_a = {owner_slot, owner.generation};
    hook->point_a = owner.cell;
    hook->facing = direction;
    hook->counter_a = hook->attack_interval = item_pattern(item).maximum;
    hook->timer_a = hook->counter_a * 4 + 24;
    hook->timer_b = 4;
    hook->ground_item = item;
    return true;
}

void step_hook(Game& game, int slot) {
    Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    Entity* owner = get_entity(game, hook.entity_a);
    if (owner == nullptr || owner->health <= 0 || hook.timer_a == 0) { release(game, slot); return; }
    if (hook.label_b != 0) { reel(game, slot, *owner); return; }
    if (hook.timer_b > 0) return;
    const Cell next = hook.cell + hook.facing;
    if (!straight_tether(game, owner->cell, next, hook.facing, hook.attack_interval)) { release(game, slot); return; }
    const Tile* tile = game.stage.at(next);
    if (tile == nullptr) { release(game, slot); return; }
    int victim = entity_at(game, next, true);
    if (victim < 0)
        for (int index = 0; index < max_entities; ++index)
            if (game.entities[static_cast<std::size_t>(index)].kind == EntityKind::GroundItem &&
                game.entities[static_cast<std::size_t>(index)].cell == next) { victim = index; break; }
    hook.cell = next;
    const bool terrain_anchor = tile->kind == TileKind::Wall || prop_blocks(tile->prop);
    if (terrain_anchor || victim >= 0) {
        hook.label_b = 2;
        hook.point_b = next;
        if (!terrain_anchor && victim >= 0) {
            const Entity& caught = game.entities[static_cast<std::size_t>(victim)];
            hook.entity_b = {victim, caught.generation};
            if (movable_by_tool(caught)) hook.label_b = 1;
        }
        hook.counter_a = 3;
        hook.timer_b = 8;
        emit_sound(game, SoundId::HookLatch, next);
        return;
    }
    if (--hook.counter_a == 0) { release(game, slot); return; }
    hook.timer_b = 4;
}
