#include "prism.hpp"
#include "../combat/beams.hpp"
#include "../item_pattern.hpp"

namespace {

void burst(Game& game, int slot) {
    const Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const Cell cell = shot.cell;
    const ItemPattern pattern = item_pattern(shot.ground_item);
    const bool piercing = shot.label_b != 0;
    remove_entity(game, {slot, shot.generation});
    // ONE PULSE: All four paths share the same snapshot, budget and damage deduplication.
    const BeamTrace trace = trace_beam_burst(game, cell, pattern.damage, pattern.blast_radius, piercing);
    resolve_beam(game, trace);
    emit_sound(game, SoundId::PrismBurst, cell);
}

} // namespace

// SLOTS: timer_a fixed fuse, timer_b flight beat; counter_a range left, attack_interval total.
// label_b saved piercing; ground_item modifier-bearing payload; entity_a owner, point_a launch.
bool launch_prism_bomb(Game& game, int owner, const Item& item, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(owner)];
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, user.cell));
    if (!shot) return false;
    shot->label_a = static_cast<int>(ProjectileKind::PrismBomb);
    shot->label_b = has_artifact(user, ArtifactKind::AllPiercing) ? 1 : 0;
    shot->sprite = Sprite::PrismBombLit;
    shot->ground_item = item; shot->ground_item.count = 1;
    shot->entity_a = {owner, user.generation};
    shot->point_a = user.cell;
    shot->facing = direction;
    shot->counter_a = shot->attack_interval = item_pattern(item).maximum;
    shot->timer_a = prism_fuse_ticks;
    shot->timer_b = projectile_step_ticks(*shot);
    shot->light = {2, 160, {184, 209, 255}};
    emit_sound(game, SoundId::PrismFuse, user.cell);
    return true;
}

void step_prism_bomb(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (shot.timer_a == 0) { burst(game, slot); return; }
    const bool urgent = shot.timer_a <= 30;
    shot.sprite = urgent ? Sprite::PrismBombReady : Sprite::PrismBombLit;
    shot.light.strength = urgent ? 300 : 160;
    if (shot.timer_a % (urgent ? 15 : 30) == 0) emit_sound(game, SoundId::PrismFuse, shot.cell);
    if (shot.counter_a == 0 || shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    // LANDING: Cover stops flight on the near side. Bodies can pass under the thrown bomb.
    if (projectile_blocked(game, next)) shot.counter_a = 0;
    else { shot.cell = next; --shot.counter_a; }
    shot.timer_b = projectile_step_ticks(shot);
    if (shot.counter_a == 0) emit_sound(game, SoundId::PrismLand, shot.cell);
}
