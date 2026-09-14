#include "behavior.hpp"
#include "hearing.hpp"
#include "foraging.hpp"
#include "dispatch.hpp"

void init_dog(Entity& dog) {
    dog.sprite = Sprite::Dog;
    dog.health = dog.max_health = 28;
    dog.move_interval = 13;
    dog.attack_interval = 45;
    dog.impassable = true;
}

void step_dog(Game& game, int slot) {
    Entity& dog = game.entities[static_cast<std::size_t>(slot)];
    if (step_hearing(game, slot)) return;
    if (step_foraging(game, slot, false)) return;
    const auto target = enemy_target(game, dog.cell, 7);
    if (!target) wander(game, slot);
    else if (distance(dog.cell, target->cell) > 1)
        approach(game, slot, target->cell);
    bite(game, slot, 6);
}
