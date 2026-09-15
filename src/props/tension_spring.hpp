#pragma once
#include "../game.hpp"
Cell tension_direction(std::uint8_t variant);
bool trigger_tension_spring(Game& game,int slot);
void step_tension_spring(Game& game,Cell cell);
