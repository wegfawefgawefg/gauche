#include "../props/scarecrow.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "dispatch.hpp"

void init_bunny(Entity& bunny) {
    bunny.sprite = Sprite::Bunny;
    bunny.health = bunny.max_health = 5;
    bunny.move_interval = 14;
    bunny.impassable = true;
}

void step_bunny(Game& game, int slot) {
    if (step_scarecrow_fear(game, slot)) return;
    if (!step_hearing(game, slot)) wander(game, slot);
}
