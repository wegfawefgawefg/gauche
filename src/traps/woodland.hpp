#pragma once
#include "../game.hpp"

bool woodland_trap(const Entity& trap);
void step_woodland_trap(Game& game, int slot);
void enter_woodland_traps(Game& game, int actor_slot);
bool hit_woodland_traps(Game& game, Cell cell, int damage, Cell source);
