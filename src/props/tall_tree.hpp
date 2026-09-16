#pragma once
#include "../game.hpp"
constexpr int tree_warn_ticks=54,tree_fall_ticks=30,tree_reach=3,tree_max_reach=5;
// TallTree: variant bits 0..1 = committed/lean direction, 2..5 = burn 0..12,
// 6..7 = height code (3,2,4,5 cells). Legacy zero stays three cells.
// growth_ticks is warning + falling countdown.
// FallenLog: bit 0 orientation, bit 2 low/walkable continuation of a crossing.
Cell tree_direction(const Prop& prop);
int tree_burn_height(const Prop& prop);
int tree_height(const Prop& prop);
std::uint8_t tree_variant(int facing,int height);
bool hit_tall_tree(Game& game,Cell cell,int damage,Cell source);
void step_tall_tree(Game& game,Cell cell);
bool valid_tree_prop(const Prop& prop);
// A fallen trunk spans up to its height; unplanted probes use legacy three.
int tree_bridge_length(const Game& game,Cell root,Cell direction);
bool lay_tree_bridge(Game& game,Cell root,Cell direction);
