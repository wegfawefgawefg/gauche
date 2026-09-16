#pragma once
#include "route.hpp"
void plan_snake_tunnel(Game& game,FloorPlan& plan);
void carve_snake_tunnel(Game& game,FloorPlan& plan,GenerationTrace* trace=nullptr);
void populate_snake_tunnel(Game& game,const FloorPlan& plan,GenerationReport* report=nullptr);
bool paint_snake_cell(Game& game,FloorPlan& plan,Cell cell,TileKind kind);
void compose_snake_terrain(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,GenerationTrace* trace);
void carve_snake_reward(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,GenerationTrace* trace);
void populate_snake_reward(Game& game,const SnakeTunnel& tunnel,GenerationReport* report);
