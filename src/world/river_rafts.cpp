#include "rivers.hpp"
#include "components.hpp"
#include "water.hpp"
#include "currents.hpp"
#include <algorithm>

// Platforms use remaining capacity after the bank and cross-room fighters.
void populate_river_rafts(Game& game,FloorPlan& plan) {
    for (const auto& river:plan.rivers) {
        const WeightedComponent amounts[]{{0,"No drifting supports",river.loop ? 0U : 3U},
            {2,"Two drifting supports",4},{3,"Three drifting supports",3},{5,"Five drifting supports",river.loop ? 2U : 0U}};
        const auto count=roll_component(game,&plan.report,GenerationFeature::River,river.component,"Rideable supports",river.path.front(),amounts);
        if (!count.value) {component_result(&plan.report,count,"Open current");continue;}
        auto sites=river.path;
        for (std::size_t i=sites.size();i>1;--i) std::swap(sites[i-1],sites[random_u32(game)%i]);
        std::vector<Cell> placed;
        for (Cell cell:sites) {
            if (static_cast<int>(placed.size())>=count.value) break;
            const auto& tile=game.stage.at_or_border(cell);
            if (!river_water(tile.kind) || prop_blocks(tile.prop) || entity_at(game,cell,false)>=0) continue;
            if (std::any_of(placed.begin(),placed.end(),[&](Cell old){return distance(old,cell)<6;})) continue;
            const WeightedComponent kinds[]{{0,"Lily pad",4},{1,"Drift log",3}};
            const auto kind=roll_component(game,&plan.report,GenerationFeature::River,count.record,"Support type",cell,kinds);
            auto* raft=get_entity(game,spawn_entity(game,EntityKind::RiverRaft,cell));
            if (!raft) {component_result(&plan.report,kind,"Entity capacity exhausted");break;}
            raft->label_a=kind.value;raft->sprite=kind.value ? Sprite::RiverLog : Sprite::RiverLily;
            raft->facing=water_current(tile);
            placed.push_back(cell);component_result(&plan.report,kind,"One player and one loose item can ride",std::array{cell});
        }
        component_result(&plan.report,count,static_cast<int>(placed.size())==count.value ?
            "Drifting supports placed" : "Partial placement: spacing, occupancy or capacity",placed);
    }
}
