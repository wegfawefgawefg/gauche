#pragma once
#include "../game.hpp"

struct FloorPlan;
constexpr int max_fissures=8,fissure_warning_ticks=66,fissure_release_ticks=90;
Cell fissure_cell(const Fissure& fissure,int offset);
bool fissure_contains(const Fissure& fissure,Cell cell);
bool fissure_hot(const Game& game,Cell cell);
bool fissure_flame(const Game& game,Cell cell);
bool cool_fissure(Game& game,Cell cell);
int extract_fissure_heat(Game& game,Cell cell,int limit);
void contact_fissure(Game& game,int slot);
void step_fissures(Game& game);
bool valid_fissures(const Game& game);
void place_fissures(Game& game,const FloorPlan& plan);
