#pragma once
#include "../game.hpp"
constexpr int pillar_warn_ticks=42, pillar_fall_ticks=24, pillar_melt_steps=12;
// variant: low two bits fall direction E/S/W/N, next four bits melted mass, high two bits height family (3/2/4/5).
// growth_ticks: warning + tip countdown; physical hp does not shrink the sprite.
Cell pillar_direction(const Prop& prop);
int pillar_height(const Prop& prop);
int pillar_mass(const Prop& prop);
int pillar_reach(const Prop& prop);
bool hit_ice_pillar(Game& game,Cell cell,int damage,Cell source);
void step_ice_pillar(Game& game,Cell cell);
bool valid_ice_pillar(const Prop& prop);
