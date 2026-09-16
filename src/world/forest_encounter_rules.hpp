#pragma once
#include "route.hpp"
#include "components.hpp"
#include <array>

// Ordinary unreserved rooms only. Landmarks and outlying encounters own separate
// pools. Pure policy shared by generation and the developer eligibility view.
struct ForestEncounterRules {
    const char* exclusion=nullptr;
    const char* specialist_note="";
    std::vector<WeightedComponent> spacing,sizes,families,specialists,wildlife;
    std::array<std::vector<WeightedComponent>,5> members;
    std::array<const char*,5> family_exclusions{};
};
ForestEncounterRules forest_encounter_rules(int floor,RoomRole role,GenerationThemes themes);
