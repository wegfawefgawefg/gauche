#pragma once
#include "../game.hpp"
constexpr int tree_warn_ticks=54,tree_fall_ticks=30,tree_reach=3;
// TallTree: variant low 2 bits committed/lean direction E/S/W/N, upper bits
// irreversible burn height 0..12. growth_ticks is warning + falling countdown.
// FallenLog: bit 0 orientation, bit 2 low/walkable continuation of a crossing.
Cell tree_direction(const Prop& prop);
int tree_burn_height(const Prop& prop);
bool hit_tall_tree(Game& game,Cell cell,int damage,Cell source);
void step_tall_tree(Game& game,Cell cell);
bool valid_tree_prop(const Prop& prop);
// A fallen trunk can span 1..3 wet/void cells between dry banks.
int tree_bridge_length(const Game& game,Cell root,Cell direction);
bool lay_tree_bridge(Game& game,Cell root,Cell direction);
