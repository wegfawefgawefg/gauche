#pragma once
#include "route.hpp"

void place_roof_scenes(Game& game,const FloorPlan& plan);
bool place_roof_span(Game& game,const FloorPlan& plan,RoofSpan roof,bool shortcut=false);
