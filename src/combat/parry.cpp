#include "parry.hpp"
#include "../entities/mirror_knight.hpp"
#include "../projectiles/projectile.hpp"

#include <algorithm>

int parry_return_damage(const Entity& defender,int incoming_damage) {
    if (defender.inventory.held()->kind!=ItemKind::ParryPan) return incoming_damage;
    return static_cast<int>(std::clamp<std::int64_t>(3LL*incoming_damage,24,100000000));
}

bool parry_active(const Entity& actor) {
    const Item& held = *actor.inventory.held();
    return actor.health > 0 && actor.sleep_ticks == 0 && actor.stun_ticks == 0 &&
        actor.block_ticks > 0 && actor.guard_slot == actor.inventory.selected &&
        (held.kind == ItemKind::ReflectingPan || held.kind==ItemKind::ParryPan) && held.count > 0 && (held.kind==ItemKind::ParryPan || held.durability > 0);
}

// CONTACT: Called only by direct ranged impacts, never melee, explosions or hazards.
bool parry_ranged_hit(Game& game, int defender_slot, Cell incoming) {
    Entity& defender = game.entities[static_cast<std::size_t>(defender_slot)];
    if (knight_reflects(defender, incoming)) {
        defender.use_flash = 8;
        emit_sound(game, SoundId::KnightReflect, defender.cell);
        return true;
    }
    if (!parry_active(defender) || (defender.facing != Cell{-incoming.x, -incoming.y} && !(defender.inventory.held()->kind==ItemKind::ParryPan && has_artifact(defender,ArtifactKind::Sweeping)))) return false;
    Item& pan = *defender.inventory.held();
    const bool permanent=pan.kind==ItemKind::ParryPan;
    if (!permanent) pan.durability -= parry_wear;
    defender.use_flash = 8;
    emit_sound(game, SoundId::PanReflect, defender.cell);
    if (!permanent && pan.durability <= 0) {
        pan = {};
        defender.block_ticks = 0;
        emit_sound(game, SoundId::PanBreak, defender.cell);
    }
    return true;
}

// FLIGHT: Reverse the same object. timer_c is its original, finite lifetime budget.
// Repeated parries may refresh a flight leg, but never extend that outer deadline.
void reflect_projectile(Entity& shot, const Entity& defender, int defender_slot) {
    shot.counter_b=parry_return_damage(defender,shot.counter_b);
    shot.cell = defender.cell;
    shot.point_a = defender.cell;
    shot.entity_a = {defender_slot, defender.generation};
    shot.facing = {-shot.facing.x, -shot.facing.y};
    shot.counter_a = shot.attack_interval;
    shot.timer_b = projectile_step_ticks(shot);
    shot.timer_a = std::min(shot.timer_c, shot.counter_a * shot.timer_b + 3);
}
