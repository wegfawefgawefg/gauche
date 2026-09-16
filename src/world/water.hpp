#pragma once

#include "../game.hpp"

struct FloorPlan;
bool shallow_water(TileKind kind);
bool supports_wall_spring(const Stage& stage,Cell cell);
bool wading_actor(const Entity& actor);
void place_water_scenes(Game& game, const FloorPlan& plan);
