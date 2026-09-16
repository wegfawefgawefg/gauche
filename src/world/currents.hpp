#pragma once
#include "../game.hpp"

// Tile current: 0 none; 1–4 east/south/west/north; 5–8 the same at double strength.
// Frozen/stilled water retains its stored direction but has no active current.
std::uint8_t make_current(Cell direction,int strength=1);
Cell stored_current_direction(const Tile& tile);
int water_current_strength(const Tile& tile);
int water_current_beat(const Tile& tile,int normal_ticks);
Cell water_current(const Tile& tile);
void step_water_currents(Game& game);
struct FloorPlan;
void place_water_currents(Game& game, const FloorPlan& plan);
