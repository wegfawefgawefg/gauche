#pragma once
#include "../game.hpp"
// label_a hunt/coil/recover; point_a/b fixed strike origin/target; counter_a/b territory x/y.
enum SnakePhase { SnakeHunt, SnakeCoil, SnakeRecover };
void init_snake(Entity& snake);
void step_snake(Game& game,int slot);
void interrupt_snake(Entity& snake);
void snake_timers(Entity& snake);
