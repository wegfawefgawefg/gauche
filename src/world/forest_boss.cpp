#include "forest_boss.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "raster.hpp"
#include "growth_carving.hpp"
#include "population_report.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include "../items/supply.hpp"
#include <algorithm>

void place_forest_boss(Game& game,FloorPlan& plan,PopulationReport* population) {
    constexpr auto feature=GenerationFeature::ForestBoss;
    if(!roll_generation_feature(game,plan,feature))return;
    std::vector<Cell> anchors;
    for(std::size_t i=0;i<plan.rooms.size();++i) {
        const auto& room=plan.rooms[i];
        if(i==0 || static_cast<int>(i)==plan.exit_room || static_cast<int>(i)==plan.objective_room || static_cast<int>(i)==plan.secret_room || room.role==RoomRole::Shrine || reserved_habitat(room))continue;
        if(distance(room.center,game.run.spawn)>=24)anchors.push_back(room.center);
    }
    for(std::size_t i=anchors.size();i>1;--i)std::swap(anchors[i-1],anchors[random_u32(game)%i]);
    auto& decision=plan.report.features.back();decision.candidate_count=static_cast<int>(anchors.size());
    for(Cell center:anchors) {
        const auto* ground=game.stage.at(center);
        if(!ground || !walkable(*ground) || ground->kind==TileKind::Water || ground->kind==TileKind::ShallowWater || entity_at(game,center,false)>=0)continue;
        const WeightedComponent forms[]{{0,"Broad feeding clearing",3},{1,"Long torn glade",3},{2,"Crooked hollow",3}};
        const auto shape=roll_component(game,&plan.report,feature,-1,"Old Growth territory",center,forms);
        const int rx=(shape.value==1 ? 11 : 8)+static_cast<int>(random_u32(game)%3),ry=5+static_cast<int>(random_u32(game)%4);
        const int bend=shape.value==2 ? 4 : 0;
        std::vector<Cell> polygon{center+Cell{-rx,-2},center+Cell{-rx/2,-ry},center+Cell{rx/2,-ry+1},center+Cell{rx,2},center+Cell{rx/2+bend,ry},center+Cell{-rx/2,ry-1}};
        if(random_u32(game)%2)for(auto& cell:polygon){const Cell d=cell-center;cell=center+Cell{-d.y,d.x};}
        if(shape.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(shape.record)];row.guide=polygon;row.guide_closed=true;}
        struct Saved {Cell cell;Tile tile;};std::vector<Saved> saved;std::vector<Cell> sites,changed;
        for(Cell cell:raster_polygon(polygon,plan.width,plan.height).cells) {
            auto* tile=game.stage.at(cell);
            if(!tile || tile->prop.kind!=PropKind::None || tile->contents!=ItemKind::None || distance(cell,game.run.spawn)<16)continue;
            bool roof=false;for(const auto& span:game.stage.roofs)if(roof_covers(span,cell))roof=true;
            if(roof)continue;
            if(tile->kind==TileKind::Wall && tile->break_rule!=BreakRule::Unbreakable && !plan.protected_cell(cell) && tile->material!=TileMaterial::Root) {
                saved.push_back({cell,*tile});*tile={TileKind::Grass};changed.push_back(cell);
            }
            if(walkable(*tile) && (tile->kind==TileKind::Grass || tile->kind==TileKind::Empty || tile->kind==TileKind::Ruin))sites.push_back(cell);
        }
        if(sites.size()<70 || !generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
            for(const auto& old:saved)*game.stage.at(old.cell)=old.tile;
            component_result(&plan.report,shape,"Rejected: insufficient dry fighting space or required route/lock");continue;
        }
        auto* boss=get_entity(game,spawn_entity(game,EntityKind::OldGrowthBear,center));
        if(!boss){for(const auto& old:saved)*game.stage.at(old.cell)=old.tile;feature_failed(plan,"Entity capacity exhausted");return;}
        boss->ground_item=supply_item(roll_item_supply(game,LootSource::Weapon,false));
        // Keep maneuvering lanes free of later blocking scenery; ordinary packs
        // still populate this room. There are no trapping doors or invisible walls.
        const auto routes=generation_walking_routes(game,plan);
        for(Cell cell:sites)if(distance(cell,center)<=3 || routes[static_cast<std::size_t>(cell.y*plan.width+cell.x)])
            plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
        for(std::size_t i=sites.size();i>1;--i)std::swap(sites[i-1],sites[random_u32(game)%i]);
        const WeightedComponent cover[]{{0,"Scattered old bones",3},{1,"Gnawed logs",4},{2,"Broken feeding ground",3}};
        const auto dressing=roll_component(game,&plan.report,feature,shape.record,"Territory remains",center,cover);
        std::vector<Cell> props;
        for(Cell cell:sites) {
            if(props.size()>=10+static_cast<std::size_t>(dressing.value)*3)break;
            if(plan.protected_cell(cell) || entity_at(game,cell,false)>=0)continue;
            const bool log=dressing.value!=0 && random_u32(game)%3==0;
            if(place_prop(game.stage,cell,log ? PropKind::RottenLog : PropKind::BonePile,static_cast<std::uint8_t>(random_u32(game)))) {
                if(log && (!generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan))){game.stage.at(cell)->prop={};continue;}
                props.push_back(cell);
            }
        }
        changed.insert(changed.end(),props.begin(),props.end());changed.push_back(center);
        component_result(&plan.report,dressing,"Breakable remains; logs shelter against a maul but can be rushed through",props);
        component_result(&plan.report,shape,"Old Growth placed; reward carried by boss; ordinary encounters retained",changed);
        decision.outcome=GenerationOutcome::Built;decision.variant="Old Growth: 640 HP, committed maul and rush";
        decision.reason="Forest 1-4 territorial boss; optional fight, native weapon and 40 gold on defeat. Exits remain independent.";
        Cell low{plan.width,plan.height},high{};for(Cell cell:sites){low.x=std::min(low.x,cell.x);low.y=std::min(low.y,cell.y);high.x=std::max(high.x,cell.x+1);high.y=std::max(high.y,cell.y+1);}decision.regions.push_back({low,high});
        if(population){auto& count=population->enemies[static_cast<std::size_t>(EntityKind::OldGrowthBear)];++count.attempted;++count.placed;}
        return;
    }
    feature_failed(plan,"No suitable unreserved, reachable fighting clearing");
}
