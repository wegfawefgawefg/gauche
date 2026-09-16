#include "forest_den.hpp"
#include "feature_roll.hpp"
#include <algorithm>

namespace {
Cell scale(Cell cell,int n) {return {cell.x*n,cell.y*n};}
bool eligible(const FloorPlan& plan,int index) {
    return index!=0 && index!=plan.exit_room && index!=plan.secret_room && index!=plan.objective_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(index)]);
}

}

void plan_forest_den(Game& game,FloorPlan& plan) {
    if (!roll_generation_feature(game,plan,GenerationFeature::BearDen)) return;
    std::vector<RouteEdge> choices;
    for (const auto edge:plan.edges)
        if (eligible(plan,edge.a) && eligible(plan,edge.b)) choices.push_back(edge);
    if (choices.empty()) { feature_failed(plan,"No adjacent eligible room pair after objective and earlier habitat reservations"); return; }
    plan.report.features.back().candidate_count=static_cast<int>(choices.size());
    auto edge=choices[random_u32(game)%choices.size()];
    // Put the sleeping hollow deeper in the route; the broad stream bank is its
    // approach. Existing corridors remain usable even if this is a through area.
    if (plan.rooms[static_cast<std::size_t>(edge.a)].depth>plan.rooms[static_cast<std::size_t>(edge.b)].depth)
        std::swap(edge.a,edge.b);
    ForestDen den;den.a=edge.a;den.b=edge.b;
    den.along=plan.rooms[static_cast<std::size_t>(edge.b)].grid-plan.rooms[static_cast<std::size_t>(edge.a)].grid;
    den.across={-den.along.y,den.along.x};
    if (random_u32(game)%2) den.across=scale(den.across,-1);
    for (int index:{den.a,den.b}) {
        auto& room=plan.rooms[static_cast<std::size_t>(index)];
        room.shape=RoomShape::BearHollow;room.role=RoomRole::Den;
        room.half_width=room.half_height=9;
    }
    plan.forest_dens.push_back(den);
    feature_reserved(plan,std::array{den.a,den.b},"Polygon banks / nested sleeping hollows");
}
