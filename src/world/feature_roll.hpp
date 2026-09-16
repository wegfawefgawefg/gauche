#pragma once
#include "route.hpp"
#include <span>

// One initial roll per feature, in the original generator order. The registry
// drives both gameplay and inspection; diagnostics never make their own rolls.
inline bool roll_generation_feature(Game& game,FloorPlan& plan,GenerationFeature feature) {
    FeatureDecision decision;
    decision.feature=feature;
    decision.denominator=feature_denominator(generation_rule(feature),game.run.floor);
    if (!decision.denominator) {
        decision.outcome=GenerationOutcome::Ineligible;
        decision.reason="Outside this feature's biome/stage window";
    } else {
        decision.roll=random_u32(game)%decision.denominator;
        decision.outcome=decision.roll==0 ? GenerationOutcome::Selected : GenerationOutcome::Missed;
        decision.reason=decision.roll==0 ? "Initial roll selected; footprint search follows" : "Initial roll missed; footprint search not attempted";
    }
    plan.report.features.push_back(std::move(decision));
    return plan.report.features.back().outcome==GenerationOutcome::Selected;
}
inline void feature_failed(FloorPlan& plan,const char* reason) {
    auto& decision=plan.report.features.back();
    decision.outcome=GenerationOutcome::Failed;decision.reason=reason;decision.candidate_count=0;
}
inline void feature_reserved(FloorPlan& plan,std::span<const int> rooms,std::string variant) {
    auto& decision=plan.report.features.back();
    decision.outcome=GenerationOutcome::Reserved;
    decision.reason="Reserved before carving; population comes later";
    decision.variant=std::move(variant);
    for (int index:rooms) {
        const auto& room=plan.rooms[static_cast<std::size_t>(index)];
        decision.regions.push_back({room.center-Cell{room.half_width,room.half_height},
                                   room.center+Cell{room.half_width+1,room.half_height+1}});
    }
}
