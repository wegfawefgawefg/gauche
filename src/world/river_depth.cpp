#include "rivers.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "growth_carving.hpp"
#include "routed_path.hpp"
#include "water.hpp"
#include <algorithm>
#include <string>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
std::vector<bool> reachable(const Stage& stage,Cell start) {
    std::vector<bool> seen(stage.tiles.size());std::vector<Cell> queue{start};
    for (std::size_t next=0;next<queue.size();++next) {
        const Cell cell=queue[next];const auto* tile=stage.at(cell);
        if (!tile || !walkable(*tile)) continue;
        const auto index=static_cast<std::size_t>(cell.y*stage.width+cell.x);
        if (seen[index]) continue;
        seen[index]=true;
        for (Cell d:sides) queue.push_back(cell+d);
    }
    return seen;
}
}

void deepen_forest_river(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    auto& river=plan.rivers.back();
    const WeightedComponent depths[]{{0,"Wadeable river",game.run.floor==1 ? 1U : 3U},
        {1,"Scattered deep pools",game.run.floor==1 ? 0U : 4U},
        {2,"Deep channel with fords",game.run.floor<3 ? 0U : 3U}};
    const auto depth=roll_component(game,&plan.report,GenerationFeature::River,river.component,"River depth",river.path.front(),depths);
    const GenerationStep checkpoint{trace,game,plan,"River depth and fords",GenerationFeature::River,depth.record};
    if (!depth.value) {component_area(&plan.report,depth,"All crossings remain shallow");return;}
    const Cell entrance=plan.rooms.front().center;
    const auto before=reachable(game.stage,entrance);
    const auto index=[&](Cell c){return static_cast<std::size_t>(c.y*plan.width+c.x);};
    const int stride=10+static_cast<int>(random_u32(game)%7);
    const int phase=static_cast<int>(random_u32(game)%static_cast<unsigned>(stride));
    std::vector<Cell> changed,fords;
    // Deep sections follow the actual routed centerline. Rolled intervals leave
    // full-width wading crossings; the outer shallows still form a visible lip.
    for (Cell c:river.channel) {
        auto& tile=*game.stage.at(c);
        if (tile.kind!=TileKind::ShallowWater || tile.prop.kind!=PropKind::None) continue;
        int near=10000;std::size_t along=0;
        for (std::size_t i=0;i<river.path.size();++i) if (distance(c,river.path[i])<near) {near=distance(c,river.path[i]);along=i;}
        const int wave=(static_cast<int>(along)+phase)%stride;
        if ((!river.loop && (along<4 || along+4>=river.path.size())) || wave<3 ||
            (depth.value==1 && wave>stride/2+2) || near>(depth.value==1 ? 0 : 1)) continue;
        tile.kind=TileKind::DeepRiver;changed.push_back(c);
    }
    // Preserve all previously reachable surviving floor, including combat banks,
    // not merely the exit. Shortest repairs prefer existing land over new fords.
    bool connected=false;
    for (int repair=0;repair<=32;++repair) {
        const auto after=reachable(game.stage,entrance);
        std::size_t missing=0;
        while (missing<before.size() && (!before[missing] || after[missing] || !walkable(game.stage.tiles[missing]))) ++missing;
        if (missing==before.size()) {connected=true;break;}
        if (repair==32) break;
        std::vector<unsigned> costs(before.size());
        for (std::size_t i=0;i<costs.size();++i) if (before[i]) {
            const auto& tile=game.stage.tiles[i];
            if (walkable(tile)) costs[i]=1;
            else if (tile.kind==TileKind::DeepRiver) costs[i]=12;
        }
        const Cell target{static_cast<int>(missing)%plan.width,static_cast<int>(missing)/plan.width};
        const auto path=route_cost_field(entrance,target,plan.width,plan.height,costs);
        if (path.empty()) break;
        for (Cell c:path) if (game.stage.at(c)->kind==TileKind::DeepRiver) {
            game.stage.at(c)->kind=TileKind::ShallowWater;fords.push_back(c);
        }
    }
    if (!connected || !generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
        for (Cell c:changed) game.stage.at(c)->kind=TileKind::ShallowWater;
        component_area(&plan.report,depth,"Depth rejected: bank access or progression could not be preserved");return;
    }
    std::erase_if(changed,[&](Cell c){return game.stage.at(c)->kind!=TileKind::DeepRiver;});
    const std::string result=changed.empty() ? "No deep cells survived crossing safeguards" :
        "Lethal without support; shallow crossings retained, "+std::to_string(fords.size())+" extra ford cells";
    component_area(&plan.report,depth,result.c_str(),changed);
    if (depth.record>=0) {
        auto& row=plan.report.components[static_cast<std::size_t>(depth.record)];
        row.guide=river.path;row.guide_cell_centers=true;row.guide_closed=river.loop;
    }
    if (!changed.empty()) {
        plan.report.features.back().variant+=" / "+std::string(depths[depth.value].name);
        plan.report.features.back().reason="Deep stretches with wading fords; all previously reachable surviving floor retained";
    }
    // Depth changes neither the current field nor the owned river footprint.
    for (Cell c:changed) plan.protected_cells[index(c)]=1;
}
