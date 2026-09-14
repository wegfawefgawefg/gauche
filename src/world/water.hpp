#pragma once

#include "../game.hpp"

struct FloorPlan;
bool shallow_water(TileKind kind);
bool wading_actor(const Entity& actor);
void cool_in_water(Game& game, int slot);
void place_water_scenes(Game& game, const FloorPlan& plan);
