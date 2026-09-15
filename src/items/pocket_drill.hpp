#pragma once
#include "catalog.hpp"
constexpr int drill_action_base=quick_slots*4;
constexpr int drill_prime_ticks=24;
const RegionalItem* pocket_drill_item(ItemKind kind);
bool pocket_drill_active(const Entity& actor);
void interrupt_pocket_drill(Entity& actor);
bool step_pocket_drill(Game& game,int slot,const Input& input);
bool valid_pocket_drill(const Entity& actor);
