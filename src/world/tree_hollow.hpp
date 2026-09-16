#pragma once
#include "route.hpp"
struct GenerationTrace;
enum TreeHollow { OpenHeart, RootGalleries, SplitHeart, WetHollow, RottenHeart };
void compose_tree_hollow(Game& game,FloorPlan& plan,GiantTree& tree,GenerationTrace* trace);
