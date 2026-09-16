#include "ant_colonies.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "population_report.hpp"
#include "routed_path.hpp"
#include "../entities/ant.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::AntColonies;
bool dry(const Tile& tile) {return walkable(tile) && tile.kind!=TileKind::Lava && tile.kind!=TileKind::ShallowWater && tile.kind!=TileKind::Spring && tile.kind!=TileKind::IceHole;}
bool site(const Game& game,const FloorPlan& plan,Cell c) {
    const auto* tile=game.stage.at(c);
    if (!tile || !dry(*tile) || tile->prop.kind!=PropKind::None || plan.protected_cell(c) || entity_at(game,c,false)>=0 || distance(c,game.run.spawn)<12) return false;
    int space=0;for (Cell d:{Cell{1,0},{-1,0},{0,1},{0,-1}}) space+=dry(game.stage.at_or_border(c+d));
    return space>=3;
}
}
void populate_ant_colonies(Game& game,FloorPlan& plan,PopulationReport* report) {
    if (!roll_generation_feature(game,plan,feature)) return;
    std::vector<Cell> sites;
    for (const auto& room:plan.rooms) {
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit || room.role==RoomRole::Secret || room.role==RoomRole::Shrine) continue;
        for (int y=-room.half_height;y<=room.half_height;++y) for (int x=-room.half_width;x<=room.half_width;++x) {
            const Cell c=room.center+Cell{x,y};if (site(game,plan,c)) sites.push_back(c);
        }
    }
    plan.report.features.back().candidate_count=static_cast<int>(sites.size());
    if (sites.empty()) {feature_failed(plan,"No free dry nest/sugar sites after inhabitants");return;}
    for (std::size_t i=sites.size();i>1;--i) std::swap(sites[i-1],sites[random_u32(game)%i]);
    std::vector<unsigned> cost(game.stage.tiles.size(),0);
    for (std::size_t i=0;i<cost.size();++i) cost[i]=dry(game.stage.tiles[i]) ? 1 : 0;
    for (const auto& e:game.entities) if (e.kind!=EntityKind::None && e.hard_blocker && e.impassable && game.stage.in_bounds(e.cell)) cost[static_cast<std::size_t>(e.cell.y*plan.width+e.cell.x)]=0;
    const bool themed=has_theme(plan.themes,GenerationTheme::Ants);
    const WeightedComponent counts[]{{1,"One colony",themed ? 1U : 6U},{2,"Two colonies",themed ? 5U : 1U},{3,"Three colonies",themed && biome_stage(game.run.floor)>=3 ? 3U : 0U}};
    const auto count=roll_component(game,&plan.report,feature,-1,"Colony count",sites.front(),counts);
    std::vector<Cell> nests;int bodies=0,sugar_units=0;
    for (int attempt=0;attempt<18 && static_cast<int>(nests.size())<count.value && attempt<static_cast<int>(sites.size());++attempt) {
        const Cell nest_cell=sites[static_cast<std::size_t>(attempt)];
        if (!site(game,plan,nest_cell)) continue;
        bool near=false;for (Cell old:nests) if (distance(old,nest_cell)<22) near=true;
        if (near) continue;
        const WeightedComponent sizes[]{{36,"Small sugar pile",3},{60,"Sugar heap",5},{90,"Spilled sugar sack",biome_stage(game.run.floor)>=2 ? 2U : 0U}};
        const auto pile=roll_component(game,&plan.report,feature,count.record,"Food route",nest_cell,sizes);
        std::vector<Cell> route;int searches=0;
        for (std::size_t pick=0;pick<sites.size() && pick<96;++pick) {
            const Cell food=sites[(pick+static_cast<std::size_t>(attempt)*13)%sites.size()];
            const int gap=distance(food,nest_cell);
            if (gap<18 || gap>55 || !site(game,plan,food)) continue;
            if (++searches>8) break;
            route=route_cost_field(nest_cell,food,plan.width,plan.height,cost);
            if (route.size()>=19 && route.size()<=90) break;
            route.clear();
        }
        if (route.empty()) {component_result(&plan.report,pile,"No connected food source at a useful distance");continue;}
        const Handle nest_handle=spawn_entity(game,EntityKind::AntNest,nest_cell);
        const Handle source_handle=spawn_entity(game,EntityKind::AntSugar,route.back());
        auto* nest=get_entity(game,nest_handle);auto* source=get_entity(game,source_handle);
        if (!nest || !source || !floor_reachable(game) || !floor_lock_required(game)) {
            remove_entity(game,nest_handle);remove_entity(game,source_handle);
            component_result(&plan.report,pile,"Fixtures rejected: capacity or progression route");continue;
        }
        nest->entity_b=source_handle;source->entity_a=nest_handle;source->counter_a=pile.value;
        std::vector<Cell> placed{nest_cell,source->cell};int workers=0;
        const int wanted=biome_stage(game.run.floor)==1 ? 4+static_cast<int>(random_u32(game)%3) : 6+static_cast<int>(random_u32(game)%4);
        const WeightedComponent roles[]{{AntWorker,"Worker",5},{AntPorter,"Sugar porter",biome_stage(game.run.floor)>=2 ? 2U : 0U}};
        // Occupants begin along their actual route, with space to pass and work.
        for (std::size_t i=2;i+2<route.size() && workers<wanted;i+=2) {
            const Cell c=route[i];if (entity_at(game,c,false)>=0 || distance(c,game.run.spawn)<9) continue;
            const auto role=roll_component(game,&plan.report,feature,pile.record,"Colony worker",c,roles);
            if (auto* ant=get_entity(game,spawn_entity(game,EntityKind::Ant,c))) {
                set_ant_role(*ant,static_cast<AntRole>(role.value));ant->entity_a=nest_handle;
                ant->move_wait=workers*3;++workers;placed.push_back(c);
                component_result(&plan.report,role,"Worker placed on food route",std::array{c});
            } else component_result(&plan.report,role,"Entity capacity exhausted");
        }
        if (!workers) {
            remove_entity(game,nest_handle);remove_entity(game,source_handle);
            component_result(&plan.report,pile,"No worker space/capacity; colony omitted");continue;
        }
        const WeightedComponent captains[]{{0,"No captain",2},{1,"Whistle captain",biome_stage(game.run.floor)>=2 ? 5U : 0U}};
        const auto captain=roll_component(game,&plan.report,feature,pile.record,"Colony leader",nest_cell,captains);
        bool leader=false;
        if (captain.value) for (Cell d:{Cell{2,0},{0,2},{-2,0},{0,-2}}) {
            const Cell c=nest_cell+d;if (!site(game,plan,c)) continue;
            if (auto* ant=get_entity(game,spawn_entity(game,EntityKind::Ant,c))) {
                set_ant_role(*ant,AntCaptain);ant->entity_a=nest_handle;placed.push_back(c);leader=true;break;
            }
        }
        component_result(&plan.report,captain,leader ? "Captain placed" : captain.value ? "No free captain site" : "Workers run without a captain");
        // Keep the established trail open through later blocking scenery passes.
        for (Cell c:route) plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
        nests.push_back(nest_cell);bodies+=workers+static_cast<int>(leader);sugar_units+=source->counter_a;
        if (pile.record>=0) plan.report.components[static_cast<std::size_t>(pile.record)].guide=route;
        component_result(&plan.report,pile,"Finite food route established; workers fetch and deliver",placed);
        cost[static_cast<std::size_t>(nest_cell.y*plan.width+nest_cell.x)]=0;
        cost[static_cast<std::size_t>(source->cell.y*plan.width+source->cell.x)]=0;
        plan.report.features.back().regions.push_back({nest_cell-Cell{2,2},nest_cell+Cell{3,3}});
    }
    component_result(&plan.report,count,"Placed colony nests",nests);
    if (report) {auto& tally=report->enemies[static_cast<std::size_t>(EntityKind::Ant)];tally.attempted+=bodies;tally.placed+=bodies;}
    auto& result=plan.report.features.back();result.outcome=nests.empty() ? GenerationOutcome::Failed : GenerationOutcome::Built;
    result.variant=std::to_string(nests.size())+" colonies, "+std::to_string(bodies)+" ants, "+std::to_string(sugar_units)+" sugar units";
    result.reason=nests.empty() ? "No viable nest/food pair" : "Conditional colony defenders; ordinary fighter groups keep their own allocation. Food and nest links survive in snapshots.";
}
