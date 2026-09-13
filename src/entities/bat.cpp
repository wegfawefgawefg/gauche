#include "behavior.hpp"
#include "dispatch.hpp"

void init_bat(Entity& bat) {
    bat.sprite = bat.kind == EntityKind::FrostBat ? Sprite::FrostBat : Sprite::Bat;
    bat.health = bat.max_health = 18;
    bat.move_interval = 15;
    bat.attack_interval = 45;
    bat.impassable = true;
}

void step_bat(Game& game, int slot) {
    Entity& bat = game.entities[static_cast<std::size_t>(slot)];
    const int target = nearest_player(game, bat.cell, 9);
    if (target < 0) wander(game, slot);
    else if (distance(bat.cell, game.entities[static_cast<std::size_t>(target)].cell) > 1)
        approach(game, slot, game.entities[static_cast<std::size_t>(target)].cell);
    bite(game, slot, 7);
}
