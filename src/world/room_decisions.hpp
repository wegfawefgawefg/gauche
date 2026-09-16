#pragma once
#include "route.hpp"
#include <span>

void begin_room_decisions(FloorPlan& plan);
void record_room_revisions(FloorPlan& plan,std::span<const RoomPlan> before,const char* source);
void finish_room_decisions(FloorPlan& plan);
