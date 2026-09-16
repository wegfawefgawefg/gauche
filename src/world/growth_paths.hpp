#pragma once
#include "../game.hpp"

// Geometry proposals only; callers decide material conversion and legal overlaps.
struct GrowthPath {
    int parent=-1;
    std::vector<Cell> points;
};
struct GrowthSettings {
    int branches=4,segments=5,min_step=4,max_step=8;
};
std::vector<GrowthPath> branching_paths(Game& game,Cell origin,Cell heading,GrowthSettings settings);
