#include "behavior.hpp"
#include "dispatch.hpp"

void init_bunny(Entity& bunny) {
    bunny.sprite = Sprite::Bunny;
    bunny.health = bunny.max_health = 5;
    bunny.move_interval = 14;
    bunny.impassable = true;
}

void step_bunny(Game& game, int slot) { wander(game, slot); }
