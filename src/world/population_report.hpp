#pragma once
#include "route.hpp"
#include <array>

// Optional generation-only counters. Never saved, hashed, or used for decisions.
struct PlacementCount { int attempted=0, placed=0, budget_blocked=0, rejected=0; };
struct SceneCount : PlacementCount { int planned=0, fallbacks=0; };
struct RangedGroup { Cell gunner{},escort{}; bool added_gunner=false; };
struct BearClearing { std::size_t room=0;unsigned kind=0;std::vector<Cell> beds; };
struct PopulationReport {
    std::vector<BearClearing> bear_clearings;
    std::vector<RoomPlan> rooms;
    std::vector<ForestDen> forest_dens;
    std::vector<Cell> bear_fishers;
    std::vector<RangedGroup> ranged_groups;
    int shelf_rooms=0, shelf_links=0;
    std::vector<ShelfReward> shelf_rewards;
    std::vector<ThawChannel> thaw_channels;
    int industry_profile=0;
    std::vector<IndustrialLink> industrial_links;
    std::array<SceneCount,static_cast<std::size_t>(RoomRole::FreightSiding)+1> scenes{};
    std::array<PlacementCount,static_cast<std::size_t>(EntityKind::Count)> enemies{};
    std::array<PlacementCount,static_cast<std::size_t>(ItemKind::Count)> supplies{};
};
