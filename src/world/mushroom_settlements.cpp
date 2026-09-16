#include "mushroom_settlements.hpp"
#include "population_report.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "raster.hpp"
#include "../props/interaction.hpp"
#include "../entities/gnome.hpp"
#include "../items/supply.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::MushroomSettlements;
bool dry(const Game& game,Cell cell) {
    const auto* t=game.stage.at(cell);return t && walkable(*t) && (t->kind==TileKind::Grass || t->kind==TileKind::Ruin || t->kind==TileKind::Empty);
}
bool site(const Game& game,const FloorPlan& plan,Cell cell) {
    if(!dry(game,cell) || plan.protected_cell(cell) || distance(cell,game.run.spawn)<12 || game.stage.at(cell)->prop.kind!=PropKind::None || entity_at(game,cell,false)>=0)return false;
    int space=0;for(Cell d:{Cell{1,0},{-1,0},{0,1},{0,-1}})space+=dry(game,cell+d) && entity_at(game,cell+d,true)<0;
    return space>=3;
}
}
void populate_mushroom_settlements(Game& game,FloorPlan& plan,PopulationReport* report) {
    if(!roll_generation_feature(game,plan,feature))return;
    std::vector<Cell> anchors;
    for(const auto& room:plan.rooms)if(room.role!=RoomRole::Entrance && room.role!=RoomRole::Exit && room.role!=RoomRole::Secret && room.role!=RoomRole::Shrine)anchors.push_back(room.center);
    for(const auto& sector:plan.open_sectors)if(!sector.ground.empty())anchors.push_back(sector.ground[sector.ground.size()/2]);
    for(std::size_t i=anchors.size();i>1;--i)std::swap(anchors[i-1],anchors[random_u32(game)%i]);
    plan.report.features.back().candidate_count=static_cast<int>(anchors.size());
    const bool major=has_theme(plan.themes,GenerationTheme::GnomeWoods);
    const int wanted=major ? 3+static_cast<int>(random_u32(game)%2) : 1+static_cast<int>(random_u32(game)%2);
    int patches=0,houses=0,residents=0,caps=0;std::vector<Cell> built;
    for(Cell anchor:anchors) {
        if(patches>=wanted)break;
        bool near=false;for(Cell old:built)if(distance(anchor,old)<18)near=true;if(near)continue;
        const WeightedComponent shapes[]{{7,"Small mushroom copse",3},{11,"Spreading grove",5},{15,"Broad mushroom wood",major?5U:1U}};
        const auto patch=roll_component(game,&plan.report,feature,-1,"Mushroom district",anchor,shapes);
        const int radius=patch.value;std::vector<Cell> polygon;
        for(Cell d:{Cell{2,0},{1,1},{0,2},{-1,1},{-2,0},{-1,-1},{0,-2},{1,-1}}) {
            const int reach=radius-2+static_cast<int>(random_u32(game)%5);
            polygon.push_back(anchor+Cell{d.x*reach/2,d.y*reach/2});
        }
        const auto footprint=raster_polygon(polygon,plan.width,plan.height);
        if(patch.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(patch.record)];row.guide=polygon;row.guide_closed=true;}
        std::vector<Cell> ground;for(Cell c:footprint.cells)if(dry(game,c) && distance(c,game.run.spawn)>=12)ground.push_back(c);
        if(ground.size()<35){component_area(&plan.report,patch,"Too little suitable dry ground");continue;}
        for(std::size_t i=ground.size();i>1;--i)std::swap(ground[i-1],ground[random_u32(game)%i]);
        const WeightedComponent settlements[]{{0,"Wild fungi; no houses",2},{1,"Isolated home",3},{3,"Small hamlet",5},{5,"Busy settlement",biome_stage(game.run.floor)>=3?3U:0U}};
        const auto village=roll_component(game,&plan.report,feature,patch.record,"Settlement size",anchor,settlements);
        Handle identity{};std::vector<Cell> homes,changed;int local_people=0;
        for(Cell at:ground) {
            if(static_cast<int>(homes.size())>=village.value)break;
            if(!site(game,plan,at) || !dry(game,at+Cell{0,1}) || entity_at(game,at+Cell{0,1},true)>=0)continue;
            bool crowded=false;for(Cell old:homes)if(distance(old,at)<5)crowded=true;if(crowded)continue;
            const Handle handle=spawn_entity(game,EntityKind::GnomeHouse,at);auto* home=get_entity(game,handle);if(!home)break;
            if(identity.slot<0)identity=handle;
            home->entity_a=identity;home->counter_a=static_cast<int>(random_u32(game)%3);
            homes.push_back(at);changed.push_back(at);++houses;
            for(int y=-1;y<=1;++y)for(int x=-1;x<=1;++x){const Cell c=at+Cell{x,y};if(game.stage.in_bounds(c))plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;}
            const WeightedComponent families[]{{0,"Vacant",2},{1,"One resident",3},{2,"Pair",5},{4,"Crowded home",biome_stage(game.run.floor)>=2?2U:0U}};
            const auto family=roll_component(game,&plan.report,feature,village.record,"Household",at,families);
            std::vector<Cell> people;
            for(int member=0;member<family.value;++member) {
                const WeightedComponent roles[]{{GnomeStick,"Stick defender",5},{GnomeBow,"Bow defender",biome_stage(game.run.floor)>=2?3U:0U},{GnomeCrossbow,"Crossbow defender",biome_stage(game.run.floor)>=3?2U:0U},{GnomeRider,"Ant rider",biome_stage(game.run.floor)>=2 ? (has_theme(plan.themes,GenerationTheme::Ants)?4U:1U):0U}};
                const auto role=roll_component(game,&plan.report,feature,family.record,"Resident role",at,roles);
                const Handle body=spawn_entity(game,EntityKind::Gnome,at);auto* person=get_entity(game,body);
                if(!person){component_result(&plan.report,role,"Entity capacity exhausted");continue;}
                set_gnome_role(*person,static_cast<GnomeRole>(role.value));person->entity_a=handle;person->label_a=GnomeHidden;person->impassable=false;person->timer_a=30+member*20;
                // Most start outdoors, but interiors can already be occupied.
                if(random_u32(game)%4!=0)for(Cell d:{Cell{0,1},{1,0},{0,-1},{-1,0},{0,2},{2,0}}){const Cell cell=at+d;if(dry(game,cell) && entity_at(game,cell,true)<0){person->cell=cell;person->impassable=true;person->label_a=GnomeCalm;break;}}
                people.push_back(person->cell);++local_people;++residents;
                component_result(&plan.report,role,person->impassable ? "Resident outdoors" : "Resident inside home",std::array{person->cell});
            }
            component_result(&plan.report,family,"Finite household placed",people);
            const WeightedComponent stores[]{{0,"Empty cupboard",5},{1,"Native supplies",3}};
            const auto supplies=roll_component(game,&plan.report,feature,family.record,"House supplies",at,stores);
            if(supplies.value)home->ground_item=supply_item(roll_item_supply(game,LootSource::Cache,false));
            component_result(&plan.report,supplies,supplies.value ? item_name(home->ground_item.kind) : "No stored item",supplies.value ? std::span<const Cell>(&at,1) : std::span<const Cell>{});
            step_gnome_house(game,handle.slot);
        }
        component_result(&plan.report,village,"Homes placed; ordinary fighter allocations retained",homes);
        std::vector<Cell> blocking;
        for(Cell at:ground) {
            auto* tile=game.stage.at(at);if(!tile || tile->prop.kind!=PropKind::None || entity_at(game,at,false)>=0)continue;
            const unsigned choice=random_u32(game)%100;
            if(choice<16 && site(game,plan,at)) {
                bool adjacent=false;for(Cell d:{Cell{1,0},{-1,0},{0,1},{0,-1}})if(prop_blocks(game.stage.at_or_border(at+d).prop))adjacent=true;
                if(!adjacent && place_prop(game.stage,at,PropKind::TallMushroom,static_cast<std::uint8_t>(random_u32(game)%6))){blocking.push_back(at);changed.push_back(at);++caps;}
            } else if(choice<38 && !plan.protected_cell(at)) {
                if(place_prop(game.stage,at,choice%5 ? PropKind::Puffball : PropKind::LanternPlant))changed.push_back(at);
            }
        }
        if(!floor_reachable(game) || !floor_lock_required(game))for(Cell at:blocking){game.stage.at(at)->prop={};--caps;changed.erase(std::remove(changed.begin(),changed.end(),at),changed.end());}
        built.push_back(anchor);++patches;
        const std::string result=std::to_string(homes.size())+" homes, "+std::to_string(local_people)+" residents; irregular fungi grown on suitable ground";
        component_area(&plan.report,patch,result.c_str(),changed);
        plan.report.features.back().regions.push_back({anchor-Cell{radius,radius},anchor+Cell{radius+1,radius+1}});
    }
    if(report){auto& count=report->enemies[static_cast<std::size_t>(EntityKind::Gnome)];count.attempted+=residents;count.placed+=residents;}
    auto& result=plan.report.features.back();result.outcome=patches?GenerationOutcome::Built:GenerationOutcome::Failed;
    result.variant=std::to_string(patches)+" districts, "+std::to_string(houses)+" homes, "+std::to_string(residents)+" residents, "+std::to_string(caps)+" large mushrooms";
    result.reason="Finite households and native cupboards; conditional defenders add to ordinary combat. Shapes, homes, occupancy, roles and supplies roll independently.";
}
