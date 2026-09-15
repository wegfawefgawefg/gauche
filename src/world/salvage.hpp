#pragma once
#include "route.hpp"

// Optional scene, composed from ordinary tiles, loose loot and breakable props.
// No required objective or unique item is ever placed inside it.
bool place_salvage_pocket(Game& game,const FloorPlan& plan,const RoomPlan& room);
