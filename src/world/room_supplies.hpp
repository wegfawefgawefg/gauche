#pragma once
#include "route.hpp"
#include <optional>

struct RoomSupplies { int threat, healing, ammunition, equipment, stashes; };
std::optional<Cell> room_space(Game& game,const RoomPlan& room,EntityKind kind=EntityKind::None);
Handle spawn_room_enemy(Game& game,const RoomPlan& room,EntityKind kind,int cost,RoomSupplies& budget);
