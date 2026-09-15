#pragma once
#include "catalog.hpp"

constexpr int rivet_action_base=quick_slots*2;
const RegionalItem* rivet_gun_item(ItemKind kind);
bool step_rivet_action(Game& game,int slot,const Input& input);
bool rivet_burst_active(const Entity& actor);
bool valid_rivet_action(const Entity& actor);
