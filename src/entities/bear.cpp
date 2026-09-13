#include "behavior.hpp"
#include "dispatch.hpp"

void init_bear(Entity& bear) {
    bear.sprite = Sprite::Bear;
    bear.health = bear.max_health = 140;
    bear.move_interval = 25;
    bear.attack_interval = 55;
    bear.impassable = true;
}

void step_bear(Game& game, int slot) {
    Entity& bear = game.entities[static_cast<std::size_t>(slot)];
    const int target = nearest_player(game, bear.cell, 9);
    if (target < 0) wander(game, slot);
    else if (distance(bear.cell, game.entities[static_cast<std::size_t>(target)].cell) > 1)
        approach(game, slot, game.entities[static_cast<std::size_t>(target)].cell);
    bite(game, slot, 24);
}
