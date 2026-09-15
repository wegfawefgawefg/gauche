#pragma once
#include "../game.hpp"

constexpr int water_heat_ticks=90;
constexpr int water_cool_ticks=1800;
bool heated_water_item(ItemKind kind);
bool fresh_water(const Game& game,Cell cell);
void step_heated_water(Game& game,Item& item,Cell cell,bool wet);
