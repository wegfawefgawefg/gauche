#include "../props/interaction.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"

namespace {

void scratch_neighbor(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    if (zombie.attack_wait > 0) return;
    if (const auto dummy = enemy_target(game, zombie.cell, 1, false)) {
        zombie.facing = dummy->cell - zombie.cell;
        hit_prop(game, dummy->cell, 5, zombie.cell);
        zombie.attack_wait = zombie.attack_interval;
        emit_sound(game, SoundId::ZombieScratch1, zombie.cell);
        return;
    }
    for (int other_slot = 0; other_slot < max_entities; ++other_slot) {
        Entity& target = game.entities[static_cast<std::size_t>(other_slot)];
        if (target.health <= 0 || distance(zombie.cell, target.cell) != 1) continue;
        const bool player = target.kind == EntityKind::Player && target.owner >= 0 &&
            target.owner < 4 && game.run.online[static_cast<std::size_t>(target.owner)];
        if (!player && target.kind != EntityKind::Chicken &&
            target.kind != EntityKind::Bunny) continue;
        zombie.facing = target.cell - zombie.cell;
        damage_entity(game, other_slot, 5, zombie.cell);
        zombie.attack_wait = zombie.attack_interval;
        emit_sound(game, SoundId::ZombieScratch1, zombie.cell);
        return;
    }
}

} // namespace

void init_zombie(Game& game, Entity& entity) {
    entity.sprite = Sprite::Zombie;
    entity.health = entity.max_health = 40;
    entity.move_interval = 48;
    entity.move_wait = static_cast<int>(random_u32(game) % 48);
    entity.attack_interval = 60;
    entity.impassable = true;
}

// SLOTS: label_b/timer_b and point_a retain the brief fall from a toppled stack.
void step_zombie(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    const auto target = enemy_target(game, zombie.cell, zombie.encounter.slot >= 0 ? 60 : 6,
        zombie.encounter.slot >= 0);
    if (step_hearing(game, slot)) {
        maybe_growl(game, slot, SoundId::ZombieGrowl1);
        return;
    }
    if (target) pursue(game, slot, target->cell);
    else wander(game, slot);
    scratch_neighbor(game, slot);
    maybe_growl(game, slot, (slot + static_cast<int>(zombie.generation)) % 2 == 0 ?
                        SoundId::ZombieGrowl1 : SoundId::ZombieGrowl2);
}
