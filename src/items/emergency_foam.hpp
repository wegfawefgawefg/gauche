#pragma once
#include "catalog.hpp"

constexpr int foam_expand_ticks=60;
constexpr int foam_life_ticks=600;
const RegionalItem* emergency_foam_item(ItemKind kind);
bool throw_emergency_foam(Game& game,int owner,Cell direction,int reach);
void step_foam_can(Game& game,int slot);
bool valid_foam_can(const Entity& actor);
bool live_foam(const Prop& prop);
void step_foam_cover(Game& game,Cell cell);
