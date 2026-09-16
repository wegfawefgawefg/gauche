#pragma once
#include "../game.hpp"
constexpr int tower_warn_ticks=60,tower_fall_ticks=30,tower_reach=4;
// variant: fixed structural load direction E/S/W/N. Hits never change it.
// growth_ticks: warning + tip countdown, hp=1 after commitment. Broken => foot only.
Cell tower_direction(const Prop& prop);
bool hit_light_tower(Game& game,Cell cell,int damage);
void step_light_tower(Game& game,Cell cell);
bool valid_light_tower(const Prop& prop);
