#pragma once
#include "../game.hpp"
constexpr int pole_warn_ticks=36,pole_fall_ticks=24,pole_reach=3;
bool streetlamp_ground(TileKind kind);
Cell streetlamp_direction(const Prop& prop);
bool hit_streetlamp(Game& game,Cell cell,int damage,Cell source);
void step_streetlamp(Game& game,Cell cell);
bool valid_streetlamp(const Prop& prop);
