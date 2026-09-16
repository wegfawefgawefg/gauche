#include "room_decisions.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::RoomComposition;
const char* shape_name(RoomShape shape) {
    constexpr const char* names[]{"Clearing","Cross","Bent hall","Twin cave","Courtyard","Pillars","Gallery","Steps",
        "Chapel nave","Ice shelf","Thaw cavern","Work hall","Excavated hall","Bear hollow","Spider cave","Snake tunnel"};
    static_assert(std::size(names)==static_cast<std::size_t>(RoomShape::SnakeTunnel)+1);
    return names[static_cast<std::size_t>(shape)];
}
std::string describe(const RoomPlan& room) {
    return std::string(room_name(room.role))+" / "+shape_name(room.shape)+" / "+
        std::to_string(room.half_width*2+1)+"x"+std::to_string(room.half_height*2+1)+
        " / turn "+std::to_string(room.turns)+(room.mirrored ? " mirrored" : "")+
        (reserved_habitat(room) ? " / reserved habitat" : " / ordinary population");
}
bool changed(const RoomPlan& a,const RoomPlan& b) {
    return a.role!=b.role || a.shape!=b.shape || a.center!=b.center || a.half_width!=b.half_width ||
        a.half_height!=b.half_height || a.turns!=b.turns || a.mirrored!=b.mirrored || a.landmark!=b.landmark;
}
}

void begin_room_decisions(FloorPlan& plan) {
    FeatureDecision decision;decision.feature=feature;
    decision.denominator=feature_denominator(generation_rule(feature),plan.report.floor);
    decision.candidate_count=static_cast<int>(plan.rooms.size());
    decision.outcome=decision.denominator ? GenerationOutcome::Selected : GenerationOutcome::Ineligible;
    decision.reason=decision.denominator ? "Base roles followed by objective, secret and habitat assignments" : "Forest room composition only";
    plan.report.features.push_back(std::move(decision));
}

void record_room_revisions(FloorPlan& plan,std::span<const RoomPlan> before,const char* source) {
    if (!feature_denominator(generation_rule(feature),plan.report.floor)) return;
    for (std::size_t i=0;i<plan.rooms.size();++i) {
        const bool added=i>=before.size();
        const auto& room=plan.rooms[i];const auto& old=added ? room : before[i];
        if (!added && !changed(old,room)) continue;
        if (plan.report.components.size()>=generation_component_limit) {plan.report.components_truncated=true;continue;}
        ComponentDecision row;row.feature=feature;row.slot=added ? "Room added" : "Room assignment";
        row.choice=std::string(source)+": "+room_name(room.role);
        row.result=added ? "New footprint: "+describe(room) : "Now "+describe(room)+"; previously "+describe(old);
        row.anchor=room.center;row.cells={room.center};row.placed=1;
        const Cell low=room.center-Cell{room.half_width,room.half_height};
        const Cell high=room.center+Cell{room.half_width+1,room.half_height+1};
        row.guide={low,{high.x,low.y},high,{low.x,high.y}};row.guide_closed=true;
        // Attach to the original roll or inserted footprint, preserving later changes as
        // an ordered sibling. No extra random draw or fabricated weighted ticket.
        for (std::size_t j=0;j<plan.report.components.size();++j) {
            const auto& original=plan.report.components[j];
            if (!added && original.feature==feature && (original.slot=="Base room role" || original.slot=="Room added") && original.anchor==old.center) {
                row.parent=static_cast<int>(j);break;
            }
        }
        plan.report.components.push_back(std::move(row));
    }
    finish_room_decisions(plan);
}

void finish_room_decisions(FloorPlan& plan) {
    for (auto& decision:plan.report.features) if (decision.feature==feature && decision.denominator) {
        int reserved=0;decision.regions.clear();
        decision.candidate_count=static_cast<int>(plan.rooms.size());
        for (const auto& room:plan.rooms) {
            reserved+=reserved_habitat(room);
            decision.regions.push_back({room.center-Cell{room.half_width,room.half_height},
                room.center+Cell{room.half_width+1,room.half_height+1}});
        }
        decision.outcome=GenerationOutcome::Built;
        decision.variant=std::to_string(plan.rooms.size())+" room footprints / "+std::to_string(reserved)+" reserved habitats";
        decision.reason="Base roles are weighted rolls. Later assignments name their source, old and new role/shape/size/orientation. They do not reroll the room.";
    }
}
