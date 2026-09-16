#pragma once
#include "../game.hpp"
#include "generation_themes.hpp"
#include <string>

// Immutable generation observations, excluded from gameplay hashes/plain state
// snapshots. Developer network snapshots may attach a bounded copy.
enum class GenerationFeature { GiantTree, TimberGrove, BearDen, SpiderCave, SnakeTunnel, RootMaze, OpenSectors, River, ForestEncounters, Themes, AntColonies, MushroomSettlements, ForestBoss, ForestBorder, ForestOutskirts, ForestFinds, Count };
enum class GenerationOutcome { Pending, Ineligible, Missed, Selected, Failed, Reserved, Built, Suppressed };
struct GenerationRegion { Cell low{},high{}; };
struct FeatureDecision {
    GenerationFeature feature{};
    GenerationOutcome outcome=GenerationOutcome::Pending;
    unsigned roll=0,denominator=0;
    int candidate_count=-1; // -1: not evaluated; four-room sites use weighted entries.
    std::string reason,variant;
    std::vector<GenerationRegion> regions;
};
struct ComponentOption { int value=0; std::string name; unsigned weight=0; };
inline constexpr std::size_t generation_component_limit=512;
struct ComponentDecision {
    GenerationFeature feature{};
    int parent=-1;
    std::string slot,choice,result;
    unsigned ticket=0,total=0;
    int placed=0;
    Cell anchor{};
    std::vector<Cell> cells,guide,rejected_cells;
    bool guide_closed=false,guide_cell_centers=false;
    std::vector<ComponentOption> options;
};
struct GenerationReport {
    std::string revision; // Empty for local reports; received reports name the host build.
    bool received=false,geometry_omitted=false;
    std::uint64_t seed=1,initial_rng=1;
    int floor=1;
    GenerationThemes themes;
    std::vector<FeatureDecision> features;
    std::vector<ComponentDecision> components;
    bool components_truncated=false;
};
struct GenerationRule {
    GenerationFeature feature;
    const char* name;
    Biome biome;
    std::array<unsigned,4> denominators;
    bool fine_steps=false; // This feature emits bounded GenerationStep checkpoints.
};
inline constexpr std::array generation_rules{
    GenerationRule{GenerationFeature::GiantTree,"Giant tree",Biome::Forest,{10,5,5,5},true},
    GenerationRule{GenerationFeature::TimberGrove,"Timber grove",Biome::Forest,{9,4,4,4},true},
    GenerationRule{GenerationFeature::BearDen,"Connected bear den",Biome::Forest,{4,3,3,3}},
    GenerationRule{GenerationFeature::SpiderCave,"Spider cave",Biome::Forest,{5,3,3,3},true},
    GenerationRule{GenerationFeature::SnakeTunnel,"Snake tunnel",Biome::Forest,{6,3,3,3},true},
    GenerationRule{GenerationFeature::RootMaze,"Root maze",Biome::Forest,{8,4,4,4}},
    GenerationRule{GenerationFeature::OpenSectors,"Open sectors",Biome::Forest,{3,2,1,2},true},
    GenerationRule{GenerationFeature::River,"Cross-room river",Biome::Forest,{2,2,2,3},true},
    GenerationRule{GenerationFeature::ForestEncounters,"Ordinary Forest encounters",Biome::Forest,{1,1,1,1}},
    GenerationRule{GenerationFeature::Themes,"Floor identity / modifiers",Biome::Forest,{1,1,1,1}},
    GenerationRule{GenerationFeature::AntColonies,"Ant colonies / food routes",Biome::Forest,{6,4,4,4}},
    GenerationRule{GenerationFeature::MushroomSettlements,"Mushroom woods / settlements",Biome::Forest,{8,5,4,4}},
    GenerationRule{GenerationFeature::ForestBoss,"Old Growth boss clearing",Biome::Forest,{0,0,0,1}},
    GenerationRule{GenerationFeature::ForestBorder,"Snowbound Forest border",Biome::Forest,{0,0,0,1}},
    GenerationRule{GenerationFeature::ForestOutskirts,"Outlying Forest encounters",Biome::Forest,{1,1,1,1},true},
    GenerationRule{GenerationFeature::ForestFinds,"Late Forest finds",Biome::Forest,{1,1,1,1},true},
};
static_assert(generation_rules.size()==static_cast<std::size_t>(GenerationFeature::Count));
inline const GenerationRule& generation_rule(GenerationFeature feature) {
    return generation_rules[static_cast<std::size_t>(feature)];
}
inline unsigned feature_denominator(const GenerationRule& rule,int floor) {
    return biome_floor(floor,rule.biome) ? rule.denominators[static_cast<std::size_t>(biome_stage(floor)-1)] : 0;
}
inline const char* generation_outcome_name(GenerationOutcome outcome) {
    switch (outcome) {
    case GenerationOutcome::Pending:return "Pending";
    case GenerationOutcome::Ineligible:return "Ineligible";
    case GenerationOutcome::Missed:return "Eligible / roll missed";
    case GenerationOutcome::Selected:return "Selected / finding space";
    case GenerationOutcome::Failed:return "Selected / placement failed";
    case GenerationOutcome::Reserved:return "Footprint reserved";
    case GenerationOutcome::Built:return "Built / applied";
    case GenerationOutcome::Suppressed:return "Suppressed";
    }
    return "Unknown";
}
inline const FeatureDecision* feature_decision(const GenerationReport& report,GenerationFeature feature) {
    for (const auto& decision:report.features) if (decision.feature==feature) return &decision;
    return nullptr;
}
