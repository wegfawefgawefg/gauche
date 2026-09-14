#include "behavior.hpp"
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
    if (step_foraging(game, slot, false)) return;
    const int target = nearest_player(game, dog.cell, 7);
    if (target < 0) wander(game, slot);
    else if (distance(dog.cell, game.entities[static_cast<std::size_t>(target)].cell) > 1)
        approach(game, slot, game.entities[static_cast<std::size_t>(target)].cell);
    bite(game, slot, 6);
}
