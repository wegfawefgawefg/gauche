#include "parry.hpp"
#include "../projectiles/projectile.hpp"

#include <algorithm>

bool parry_active(const Entity& actor) {
    const Item& held = *actor.inventory.held();
    return actor.health > 0 && actor.sleep_ticks == 0 && actor.stun_ticks == 0 &&
        actor.block_ticks > 0 && actor.guard_slot == actor.inventory.selected &&
        held.kind == ItemKind::ReflectingPan && held.count > 0 && held.durability > 0;
}

// CONTACT: Called only by direct ranged impacts, never melee, explosions or hazards.
bool parry_ranged_hit(Game& game, int defender_slot, Cell incoming) {
    Entity& defender = game.entities[static_cast<std::size_t>(defender_slot)];
    if (!parry_active(defender) || defender.facing != Cell{-incoming.x, -incoming.y}) return false;
    Item& pan = *defender.inventory.held();
    pan.durability -= parry_wear;
    defender.use_flash = 8;
    emit_sound(game, SoundId::PanReflect, defender.cell);
    if (pan.durability <= 0) {
        pan = {};
        defender.block_ticks = 0;
        emit_sound(game, SoundId::PanBreak, defender.cell);
    }
    return true;
}

// FLIGHT: Reverse the same object. timer_c is its original, finite lifetime budget.
// Repeated parries may refresh a flight leg, but never extend that outer deadline.
void reflect_projectile(Entity& shot, const Entity& defender, int defender_slot) {
    shot.cell = defender.cell;
    shot.point_a = defender.cell;
    shot.entity_a = {defender_slot, defender.generation};
    shot.facing = {-shot.facing.x, -shot.facing.y};
    shot.counter_a = shot.attack_interval;
    shot.timer_b = projectile_step_ticks(shot);
    shot.timer_a = std::min(shot.timer_c, shot.counter_a * shot.timer_b + 3);
}
