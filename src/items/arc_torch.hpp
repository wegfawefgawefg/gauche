#pragma once
#include "catalog.hpp"

constexpr int arc_action_base=quick_slots*3;
constexpr int arc_prime_ticks=18;
const RegionalItem* arc_torch_item(ItemKind kind);
bool arc_torch_active(const Entity& actor);
bool step_arc_torch(Game& game,int slot,const Input& input);
bool valid_arc_torch(const Entity& actor);
