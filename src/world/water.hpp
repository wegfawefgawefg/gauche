#pragma once

#include "../game.hpp"

struct FloorPlan;
bool shallow_water(TileKind kind);
inline bool river_water(TileKind kind) { return shallow_water(kind) || kind==TileKind::DeepRiver; }
bool river_swimmer(const Entity& actor);
bool deep_river_contact(Game& game,int slot);
bool finish_deep_river_death(Game& game,int slot);
void step_deep_river_contacts(Game& game);
bool supports_wall_spring(const Stage& stage,Cell cell);
bool wading_actor(const Entity& actor);
void place_water_scenes(Game& game, const FloorPlan& plan);
