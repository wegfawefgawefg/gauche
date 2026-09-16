#pragma once
#include "components.hpp"
void compose_landmark_supplies(Game& game,GenerationReport* report,GenerationFeature feature,
    std::span<const Cell> ground,Cell cache,bool optional_reward=false);
