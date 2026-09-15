#pragma once
#include "../game.hpp"
bool nail_board_trap(const Entity& trap);
void step_nail_board_trap(Game& game,int slot);
void enter_nail_boards(Game& game,int actor_slot);
bool hit_nail_boards(Game& game,Cell cell,int damage,Cell source);
Item recoverable_nail_board(const Game& game,Cell cell);
int release_nail_board(Game& game,Cell cell);
bool valid_nail_board(const Entity& trap);
