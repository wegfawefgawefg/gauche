#pragma once

#include "../game.hpp"

constexpr int wolf_call_ticks = 300;
bool call_wolf(Game& game, int caller, Cell direction, int reach);
const Entity* called_wolf_prey(Game& game, Entity& wolf);
