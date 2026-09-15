#include "frost.hpp"
#include "projectile.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

namespace {

void disperse(Game& game, int slot, Cell cell, bool melted = false) {
    remove_entity(game, {slot, game.entities[static_cast<std::size_t>(slot)].generation});
    emit_sound(game, melted ? SoundId::IceMelt : SoundId::FrostPuffHit, cell);
}

} // namespace

// SLOTS: counter_a remaining cells, counter_b damage, timer_a life, timer_b travel.
// entity_a generation-checked owner; facing stays cardinal for the whole puff.
bool launch_frost_puff(Game& game, int owner, Cell direction, int reach) {
    const Entity& bat = game.entities[static_cast<std::size_t>(owner)];
    Entity* puff = get_entity(game, spawn_entity(game, EntityKind::Projectile, bat.cell));
    if (puff == nullptr) return false;
    puff->sprite = Sprite::FrostPuff;
    puff->label_a = static_cast<int>(ProjectileKind::FrostPuff);
    puff->entity_a = {owner, bat.generation};
    puff->facing = direction;
    puff->counter_a = puff->attack_interval = std::clamp(reach, 1, 12);
    puff->counter_b = 6;
    puff->timer_b = frost_puff_step_ticks;
    puff->timer_a = puff->counter_a * frost_puff_step_ticks + 1;
    return true;
}

void step_frost_puff(Game& game, int slot) {
    Entity& puff = game.entities[static_cast<std::size_t>(slot)];
    if (hot_cell(game, puff.cell)) { disperse(game, slot, puff.cell, true); return; }
    if (puff.timer_a == 0 || puff.counter_a == 0) { disperse(game, slot, puff.cell); return; }
    if (puff.timer_b > 0) return;
    const Cell next = puff.cell + puff.facing;
    if (projectile_blocked(game,next,false,true)) {
        // COVER: Capture obstruction before damaging it; no chilling through broken cover.
        hit_prop(game, next, puff.counter_b, puff.cell);
        disperse(game, slot, puff.cell);
        return;
    }
    if (hot_cell(game, next)) { disperse(game, slot, next, true); return; }
    const int victim = entity_at(game, next, true);
    if (victim >= 0) {
        Entity& target = game.entities[static_cast<std::size_t>(victim)];
        if (Handle{victim, target.generation} != puff.entity_a) {
            const int health = target.health;
            damage_entity(game, victim, puff.counter_b, puff.cell, true, puff.entity_a);
            if (target.health < health) apply_chill(target, 120);
            disperse(game, slot, next);
            return;
        }
    }
    puff.cell = next;
    --puff.counter_a;
    puff.timer_b = frost_puff_step_ticks;
    if (puff.counter_a == 0) disperse(game, slot, next);
}
