#pragma once
#include "../game.hpp"
inline constexpr std::array<Cell,4> rail_directions{{{1,0},{0,1},{-1,0},{0,-1}}};
bool live_rail_points(const Prop& prop);
Cell rail_exit(const Stage& stage,Cell cell,Cell incoming);
bool turn_rail_points(Game& game,Cell cell);
