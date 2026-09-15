#pragma once
#include "catalog.hpp"

constexpr int thaw_fuse_ticks=120;
const RegionalItem* thaw_charge_item(ItemKind kind);
bool thawable_wall(const Stage& stage, Cell cell);
bool place_thaw_charge(Game& game, int slot);
void step_thaw_charge(Game& game, int slot);
bool valid_thaw_charge(const Entity& actor);
