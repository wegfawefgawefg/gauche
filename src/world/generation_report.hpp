#pragma once
#include "../game.hpp"
#include "generation_themes.hpp"
#include <string>

// Local observations only: shared immutably with an inspected/played Game and
// omitted from hashes and network snapshots. Received snapshots have no report.
enum class GenerationFeature { GiantTree, TimberGrove, BearDen, SpiderCave, SnakeTunnel, RootMaze, OpenSectors, River, ForestEncounters, Themes, AntColonies, MushroomSettlements, ForestBoss, ForestBorder, Count };
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
struct ComponentDecision {
    GenerationFeature feature{};
    int parent=-1;
    std::string slot,choice,result;
    unsigned ticket=0,total=0;
    int placed=0;
    Cell anchor{};
    std::vector<Cell> cells,guide,rejected_cells;
    bool guide_closed=false;
    std::vector<ComponentOption> options;
};
struct GenerationReport {
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
};
inline constexpr std::array generation_rules{
    GenerationRule{GenerationFeature::GiantTree,"Giant tree",Biome::Forest,{10,5,5,5}},
    GenerationRule{GenerationFeature::TimberGrove,"Timber grove",Biome::Forest,{9,4,4,4}},
    GenerationRule{GenerationFeature::BearDen,"Connected bear den",Biome::Forest,{4,3,3,3}},
    GenerationRule{GenerationFeature::SpiderCave,"Spider cave",Biome::Forest,{5,3,3,3}},
    GenerationRule{GenerationFeature::SnakeTunnel,"Snake tunnel",Biome::Forest,{6,3,3,3}},
    GenerationRule{GenerationFeature::RootMaze,"Root maze",Biome::Forest,{8,4,4,4}},
    GenerationRule{GenerationFeature::OpenSectors,"Open sectors",Biome::Forest,{3,2,1,2}},
    GenerationRule{GenerationFeature::River,"Cross-room river",Biome::Forest,{2,2,2,3}},
    GenerationRule{GenerationFeature::ForestEncounters,"Ordinary Forest encounters",Biome::Forest,{1,1,1,1}},
    GenerationRule{GenerationFeature::Themes,"Floor identity / modifiers",Biome::Forest,{1,1,1,1}},
    GenerationRule{GenerationFeature::AntColonies,"Ant colonies / food routes",Biome::Forest,{6,4,4,4}},
    GenerationRule{GenerationFeature::MushroomSettlements,"Mushroom woods / settlements",Biome::Forest,{8,5,4,4}},
    GenerationRule{GenerationFeature::ForestBoss,"Old Growth boss clearing",Biome::Forest,{0,0,0,1}},
    GenerationRule{GenerationFeature::ForestBorder,"Snowbound Forest border",Biome::Forest,{0,0,0,1}},
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
