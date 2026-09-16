#pragma once
#include "route.hpp"
#include <array>

// Optional generation-only counters. Never saved, hashed, or used for decisions.
struct PlacementCount { int attempted=0, placed=0, budget_blocked=0, rejected=0; };
struct SceneCount : PlacementCount { int planned=0, fallbacks=0; };
struct PopulationReport {
    int shelf_rooms=0, shelf_links=0;
    std::vector<ShelfReward> shelf_rewards;
    std::vector<ThawChannel> thaw_channels;
    int industry_profile=0;
    std::vector<IndustrialLink> industrial_links;
    std::array<SceneCount,static_cast<std::size_t>(RoomRole::FreightSiding)+1> scenes{};
    std::array<PlacementCount,static_cast<std::size_t>(EntityKind::Count)> enemies{};
    std::array<PlacementCount,static_cast<std::size_t>(ItemKind::Count)> supplies{};
};
