#include "behavior.hpp"
#include "dispatch.hpp"

void init_wolf(Entity& wolf) {
    wolf.sprite = Sprite::Wolf;
    wolf.health = wolf.max_health = 45;
    wolf.move_interval = 10;
    wolf.attack_interval = 38;
    wolf.impassable = true;
}

void step_wolf(Game& game, int slot) {
    Entity& wolf = game.entities[static_cast<std::size_t>(slot)];
    const int target = nearest_player(game, wolf.cell, 9);
    if (target < 0) wander(game, slot);
    else if (distance(wolf.cell, game.entities[static_cast<std::size_t>(target)].cell) > 1)
        approach(game, slot, game.entities[static_cast<std::size_t>(target)].cell);
    bite(game, slot, 11);
}
