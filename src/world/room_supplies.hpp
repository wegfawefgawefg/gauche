#pragma once
#include "route.hpp"
#include "population_report.hpp"
#include <optional>

struct RoomSupplies { int threat, healing, ammunition, equipment, stashes; PopulationReport* report=nullptr; };
std::vector<Cell> room_spaces(const Game& game,const RoomPlan& room,EntityKind kind=EntityKind::None);
std::optional<Cell> room_space(Game& game,const RoomPlan& room,EntityKind kind=EntityKind::None);
Handle spawn_room_enemy(Game& game,const RoomPlan& room,EntityKind kind,int cost,RoomSupplies& budget);
