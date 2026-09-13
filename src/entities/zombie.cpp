#include "behavior.hpp"
#include "dispatch.hpp"

namespace {

void scratch_neighbor(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    if (zombie.attack_wait > 0) return;
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
        zombie.sprite = Sprite::ZombieScratch1;
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

void step_zombie(Game& game, int slot) {
    Entity& zombie = game.entities[static_cast<std::size_t>(slot)];
    const Cell before = zombie.cell;
    // A scratch pose lingers until the zombie takes another step.
    wander(game, slot);
    if (zombie.cell != before) zombie.sprite = Sprite::Zombie;
    scratch_neighbor(game, slot);
    maybe_growl(game, slot, (slot + static_cast<int>(zombie.generation)) % 2 == 0 ?
                        SoundId::ZombieGrowl1 : SoundId::ZombieGrowl2);
}
