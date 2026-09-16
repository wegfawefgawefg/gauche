#include "tall_trees.hpp"
#include "components.hpp"
#include "feature_roll.hpp"
#include "generation_trace.hpp"
#include "growth_carving.hpp"
#include "../props/tall_tree.hpp"
#include "../props/interaction.hpp"
#include "../items/folded_bridge.hpp"
#include "../scenery/roof.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr auto feature=GenerationFeature::ForestTrees;
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
bool eligible(const RoomPlan& room) {
    return !reserved_habitat(room) && (room.role==RoomRole::Clearing || room.role==RoomRole::Thicket ||
        room.role==RoomRole::Brook || room.role==RoomRole::Orchard || room.role==RoomRole::Den);
}
bool space(const Game& game,const FloorPlan& plan,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !bridge_bank(*tile) || surface_wet(*tile) || tile->prop.kind!=PropKind::None ||
        entity_at(game,cell,false)>=0 || plan.protected_cell(cell)) return false;
    for(const auto& roof:game.stage.roofs)if(roof_covers(roof,cell))return false;
    for(int y=-2;y<=2;++y)for(int x=-2;x<=2;++x)
        if(std::abs(x)+std::abs(y)<3 && game.stage.at_or_border(cell+Cell{x,y}).prop.kind==PropKind::TallTree)return false;
    for (const Entity& actor:game.entities) if (distance(actor.cell,cell)<5 &&
        (actor.kind==EntityKind::Player || actor.kind==EntityKind::Door || actor.kind==EntityKind::Key ||
         actor.kind==EntityKind::Switch || actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate ||
         actor.kind==EntityKind::Bear || actor.kind==EntityKind::OldGrowthBear)) return false;
    return true;
}
bool place(Game& game,const FloorPlan& plan,Cell cell,int facing,int height) {
    if (!space(game,plan,cell) || !place_prop(game.stage,cell,PropKind::TallTree,tree_variant(facing,height))) return false;
    bool valid=ground_neighbors_connected(game.stage,cell) && floor_reachable(game);
    // Cutting can override the planted lean. Check every possible future bridge.
    for (Cell dir:directions) {
        if(!valid)break;
        const int length=tree_bridge_length(game,cell,dir);if (!length) continue;
        std::array<Tile,tree_max_reach> saved{};
        for (int i=0;i<length;++i) saved[static_cast<std::size_t>(i)]=*game.stage.at(cell+Cell{dir.x*(i+1),dir.y*(i+1)});
        lay_tree_bridge(game,cell,dir);
        const auto tree=game.stage.at(cell)->prop;game.stage.at(cell)->prop={};
        valid=floor_lock_required(game);
        game.stage.at(cell)->prop=tree;
        for (int i=0;i<length;++i) *game.stage.at(cell+Cell{dir.x*(i+1),dir.y*(i+1)})=saved[static_cast<std::size_t>(i)];
    }
    if (!valid) game.stage.at(cell)->prop={};
    return valid;
}
}

void place_tall_trees(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    if(!roll_generation_feature(game,plan,feature))return;
    int rooms=0,total=0;
    for(const auto& room:plan.rooms) {
        if(!eligible(room))continue;
        ++rooms;
        const bool wooded=room.role==RoomRole::Thicket || has_theme(plan.themes,GenerationTheme::Timber) ||
            has_theme(plan.themes,GenerationTheme::Overgrowth);
        const WeightedComponent forms[]{{0,"Open understory",wooded?1U:4U},{1,"Lone tree",3},
            {2,"Scattered trees",4},{3,"Close stand",wooded?5U:2U},{4,"Broken tree ring",2}};
        const auto form=roll_component(game,&plan.report,feature,-1,"Ordinary tree pattern",room.center,forms);
        const GenerationStep step{trace,game,plan,"Ordinary tree stand",feature,form.record};
        if(!form.value){component_result(&plan.report,form,"Open outcome; existing inhabitants and low scenery retained");continue;}
        const WeightedComponent counts[]{{2,"Two trees",4},{3,"Three trees",4},{5,"Five trees",wooded?5U:2U},{7,"Seven trees",wooded?3U:1U}};
        const auto number=form.value==1 ? ComponentRoll{1,-1} :
            roll_component(game,&plan.report,feature,form.record,"Stand size",room.center,counts);
        const WeightedComponent profiles[]{{0,"Young growth",3},{1,"Mixed heights",5},{2,"High stand",wooded?3U:1U}};
        const auto profile=roll_component(game,&plan.report,feature,form.record,"Height mixture",room.center,profiles);
        std::vector<Cell> sites;
        for(int y=-room.half_height;y<=room.half_height;++y)for(int x=-room.half_width;x<=room.half_width;++x) {
            const Cell cell=room.center+Cell{x,y};if(space(game,plan,cell))sites.push_back(cell);
        }
        for(std::size_t n=sites.size();n>1;--n)std::swap(sites[n-1],sites[random_u32(game)%n]);
        const Cell center=sites.empty()?room.center:sites.front();
        const int radius=3+static_cast<int>(random_u32(game)%3);
        std::vector<Cell> planted,rejected;
        const WeightedComponent heights[]{{2,"Two-cell tree",profile.value==2?0U:4U},
            {3,"Three-cell tree",profile.value==2?1U:5U},{4,"Four-cell tree",profile.value==0?0U:4U},
            {5,"Five-cell tree",profile.value==0?0U:2U}};
        int attempts=0;
        for(Cell cell:sites) {
            if(static_cast<int>(planted.size())>=number.value || attempts>=24)break;
            const float dx=static_cast<float>(cell.x-center.x),dy=static_cast<float>(cell.y-center.y);
            const float radial=std::hypot(dx,dy);
            if((form.value==3 && radial>5) || (form.value==4 && std::abs(radial-static_cast<float>(radius))>.8F) ||
                !space(game,plan,cell))continue;
            ++attempts;
            const auto height=roll_component(game,&plan.report,feature,profile.record,"Tree height / fall reach",cell,heights);
            const int facing=static_cast<int>(random_u32(game)%4);
            if(place(game,plan,cell,facing,height.value)) {
                planted.push_back(cell);component_result(&plan.report,height,"Planted; fall and bridge reach match height",std::span<const Cell>(&cell,1));
            } else {
                rejected.push_back(cell);component_result(&plan.report,height,"Rejected: would split walking ground or bypass an exit lock");
            }
        }
        const char* result=planted.empty() ? "No compatible ground after occupancy, route and lock checks" :
            planted.size()==static_cast<std::size_t>(number.value) ? "Stand placed in remaining ordinary ground" : "Partial stand; occupied/protected ground and route/lock checks reduce count";
        component_result(&plan.report,form,result,planted);component_result(&plan.report,number,result,planted);
        component_result(&plan.report,profile,"Height pool applied independently at each attempted tree",planted);
        if(form.record>=0)plan.report.components[static_cast<std::size_t>(form.record)].rejected_cells=std::move(rejected);
        if(!planted.empty())plan.report.features.back().regions.push_back({room.center-Cell{room.half_width,room.half_height},room.center+Cell{room.half_width+1,room.half_height+1}});
        total+=static_cast<int>(planted.size());
    }
    auto& decision=plan.report.features.back();decision.candidate_count=rooms;
    decision.outcome=total ? GenerationOutcome::Built : GenerationOutcome::Failed;
    decision.reason="Independent ordinary-room stands after inhabitants; reserved habitats, occupied cells, walking connections and exit locks preserved";
    decision.variant=std::to_string(total)+" trees across "+std::to_string(rooms)+" eligible rooms";
}
