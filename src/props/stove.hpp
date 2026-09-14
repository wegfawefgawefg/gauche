#pragma once
#include "../game.hpp"
constexpr int stove_fuel_limit = 7200;
bool stove_lit(const Prop& prop);
bool prop_has_flame(const Prop& prop);
bool light_stove(Game& game, Cell cell);
bool douse_stove(Game& game, Cell cell);
bool feed_stove(Game& game, Cell cell);
void step_stove(Game& game, Cell cell);
