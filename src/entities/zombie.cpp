#include "behavior.hpp"
#include "dispatch.hpp"

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
    // The scratch is a one-step pose; wandering restores the ordinary zombie.
    wander(game, slot);
    zombie.sprite = Sprite::Zombie;
    bite(game, slot, 5);
    if (zombie.attack_wait == zombie.attack_interval)
        zombie.sprite = Sprite::ZombieScratch1;
    maybe_growl(game, slot, (slot + static_cast<int>(zombie.generation)) % 2 == 0 ?
                        SoundId::ZombieGrowl1 : SoundId::ZombieGrowl2);
}
