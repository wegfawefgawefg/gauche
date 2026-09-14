#include "ice_brick.hpp"
#include "../item_pattern.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../combat/parry.hpp"

namespace {

void shatter(Game& game, int slot, Cell cell, bool melted = false) {
    const Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (!melted && game.impact_count < static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)] =
            {cell, shot.cell, Sprite::IceBlock, shot.counter_b, true, PropKind::IceBlock};
    emit_sound(game, melted ? SoundId::IceMelt : SoundId::IceBlockBreak, cell);
    remove_entity(game, {slot, shot.generation});
}

} // namespace

bool launch_ice_brick(Game& game, int owner_slot, const Item& item, Cell direction) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, owner.cell));
    if (!shot) return false;
    const ItemPattern pattern = item_pattern(item);
    shot->label_a = static_cast<int>(ProjectileKind::IceBrick);
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->counter_b = pattern.damage;
    shot->label_b = has_artifact(owner, ArtifactKind::AllPiercing) ? 1 : 0;
    shot->timer_b = projectile_step_ticks(*shot);
    shot->timer_a = shot->counter_a * shot->timer_b + 1;
    shot->timer_c = shot->timer_a * 4;
    shot->point_a = owner.cell;
    shot->entity_a = {owner_slot, owner.generation};
    shot->facing = direction;
    shot->ground_item = item;
    shot->ground_item.count = 1;
    shot->sprite = Sprite::IceBlock;
    emit_sound(game, SoundId::IceBrickThrow, owner.cell);
    return true;
}

void step_ice_brick_flight(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (hot_cell(game, shot.cell)) { shatter(game, slot, shot.cell, true); return; }
    if (shot.timer_a == 0 || shot.timer_c == 0 || shot.counter_a == 0) {
        shatter(game, slot, shot.cell); return;
    }
    if (shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    // OBSTRUCTION: A broken block still stops this throw; no actor hit through cover.
    if (projectile_blocked(game, next)) {
        hit_prop(game, next, shot.counter_b, shot.cell);
        hit_terrain(game, next, shot.cell, shot.counter_b, 0);
        shatter(game, slot, shot.cell);
        return;
    }
    if (hot_cell(game, next)) { shatter(game, slot, next, true); return; }
    const int victim = entity_at(game, next, true);
    if (victim >= 0) {
        const Entity& target = game.entities[static_cast<std::size_t>(victim)];
        if (Handle{victim, target.generation} != shot.entity_a) {
            if (parry_ranged_hit(game, victim, shot.facing)) {
                reflect_projectile(shot, target, victim);
                return;
            }
            damage_entity(game, victim, shot.counter_b, shot.cell);
            if (shot.label_b == 0) { shatter(game, slot, next); return; }
        }
    }
    hit_prop(game, next, shot.counter_b, shot.cell);
    shot.cell = next;
    --shot.counter_a;
    shot.timer_b = projectile_step_ticks(shot);
    if (shot.counter_a == 0) shatter(game, slot, next);
}
