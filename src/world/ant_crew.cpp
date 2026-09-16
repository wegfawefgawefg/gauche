#include "ant_crew.hpp"
#include "components.hpp"
#include "../entities/ant_hauling.hpp"

int place_ant_crew(Game& game,FloorPlan& plan,Handle home,Handle captain,int parent) {
    auto* nest=get_entity(game,home);if (!nest) return 0;
    auto* source=get_entity(game,nest->entity_b);if (!source) return 0;
    const WeightedComponent options[]{{0,"Individual gatherers",4},{1,"Two-ant rope crew",biome_stage(game.run.floor)>=2 ? 5U : 0U}};
    const auto roll=roll_component(game,&plan.report,GenerationFeature::AntColonies,parent,"Heavy hauling",source->cell,options);
    if (!roll.value) {component_result(&plan.report,roll,"No heavy crew selected");return 0;}
    const int turn=static_cast<int>(random_u32(game)%4);
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    for (int n=0;n<4;++n) {
        const Cell d=directions[(n+turn)%4],at=source->cell+Cell{d.x*2,d.y*2};
        if (plan.protected_cell(at) || distance(at,game.run.spawn)<12 || entity_at(game,at,false)>=0) continue;
        const auto path=ant_load_route(game,at,nest->cell);
        if (path.size()<5) continue;
        const Cell forward=path[1]-at,side{-forward.y,forward.x};
        const Cell left=at+forward-side,right=at+forward+side;
        if (entity_at(game,left,false)>=0 || entity_at(game,right,false)>=0) continue;
        const Handle load=spawn_entity(game,EntityKind::AntLoad,at);
        const Handle a=spawn_entity(game,EntityKind::Ant,left),b=spawn_entity(game,EntityKind::Ant,right);
        auto* cargo=get_entity(game,load);auto* first=get_entity(game,a);auto* second=get_entity(game,b);
        if (!cargo || !first || !second) {
            remove_entity(game,load);remove_entity(game,a);remove_entity(game,b);
            component_result(&plan.report,roll,"No capacity for intact three-body crew");return 0;
        }
        cargo->entity_a=home;cargo->entity_b=captain;cargo->facing=forward;
        set_ant_role(*first,AntPuller);set_ant_role(*second,AntPuller);
        first->entity_a=second->entity_a=load;first->label_b=0;second->label_b=1;
        // Preserve only the actual formation corridor against later scenery.
        for (Cell c:path) for(int y=-1;y<=1;++y) for(int x=-1;x<=1;++x)
            plan.protected_cells[static_cast<std::size_t>((c.y+y)*plan.width+c.x+x)]=1;
        if (roll.record>=0) plan.report.components[static_cast<std::size_t>(roll.record)].guide=path;
        component_result(&plan.report,roll,"Two pullers and load placed; finite sugar shared with workers",std::array{at,left,right});return 2;
    }
    component_result(&plan.report,roll,"No wide connected cargo route with free puller stances");return 0;
}
