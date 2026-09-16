#include "forest_den.hpp"
#include "bear_clearings.hpp"
#include "components.hpp"
#include "landmark_supplies.hpp"
#include "ground_items.hpp"
#include "water.hpp"
#include "../entities/attacks.hpp"
#include "../entities/bear_family.hpp"
#include "../entities/bear_fishing.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::BearDen;
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
bool vacant(const Game& game,Cell c) {
    const auto& tile=game.stage.at_or_border(c);
    return (tile.kind==TileKind::Grass || tile.kind==TileKind::Empty) &&
        tile.prop.kind==PropKind::None && entity_at(game,c,false)<0;
}
std::vector<Cell> ground(const Game& game,const RoomPlan& room) {
    std::vector<Cell> cells;
    for(int y=-room.half_height;y<=room.half_height;++y)
        for(int x=-room.half_width;x<=room.half_width;++x) {
            const Cell c=room.center+Cell{x,y};
            if(vacant(game,c))cells.push_back(c);
        }
    return cells;
}

ComponentRoll residents(Game& game,const ForestDen& den,GenerationReport* report) {
    const WeightedComponent groups[]{{0,"Sleeping family",6},{1,"Adult gathering",3},{2,"Old solitary bear",2}};
    const auto group=roll_component(game,report,feature,den.component,"Hollow residents",den.cache,groups);
    std::vector<Cell> beds;
    for(Cell c:den.beds)if(vacant(game,c))beds.push_back(c);
    if(group.value==2 && !beds.empty())beds.resize(1);
    if(group.value!=2)populate_bear_beds(game,beds,group.value==0);
    else if(!beds.empty()) {
        if(auto* bear=get_entity(game,spawn_entity(game,EntityKind::Bear,beds[0]))) {
            set_bear_role(*bear,BearOld);apply_sleep(*bear,36000);
            place_prop(game.stage,beds[0],PropKind::BearBed);
        }
    }
    std::erase_if(beds,[&](Cell c){const int slot=entity_at(game,c,false);return slot<0 || game.entities[static_cast<std::size_t>(slot)].kind!=EntityKind::Bear;});
    component_result(report,group,beds.empty() ? "No vacant bed or entity capacity" : "Sleeping residents; separate active approach encounters",beds);
    return group;
}

void stream(Game& game,std::span<const Cell> sites,int parent,Cell anchor,GenerationReport* report) {
    struct Bank { Cell stand,fish; };std::vector<Bank> banks;
    for(Cell c:sites)if(vacant(game,c))for(Cell d:sides) {
        const Cell fish=c+d;
        if(shallow_water(game.stage.at_or_border(fish).kind) && entity_at(game,fish,false)<0)
            banks.push_back({c,fish});
    }
    const WeightedComponent activity[]{{0,"Quiet stream",3},{1,"Fish shoal",4},{2,"Fishing bear",banks.empty() ? 0U : 3U}};
    const auto roll=roll_component(game,report,feature,parent,"Stream activity",anchor,activity);
    if(!roll.value){component_result(report,roll,"No stream inhabitants");return;}
    if(banks.empty()){component_result(report,roll,"No vacant shallow-water bank survived");return;}
    const auto bank=banks[random_u32(game)%banks.size()];
    const WeightedComponent stocks[]{{3,"Three fish",3},{5,"Five fish",4},{6,"Six fish",2}};
    const auto stock=roll_component(game,report,feature,roll.record,"Finite fish stock",bank.fish,stocks);
    const auto fish=place_ground_item(game,bank.fish,ItemKind::RiverFish,stock.value);
    if(!get_entity(game,fish)) {
        component_result(report,stock,"Entity capacity exhausted");
        component_result(report,roll,"No fish placed; fishing bear omitted");return;
    }
    component_result(report,stock,"Real fish; can be caught or stolen",std::array{bank.fish});
    std::vector<Cell> placed{bank.fish};
    const char* result="Fish shoal without a bear";
    if(roll.value==2) {
        if(auto* bear=get_entity(game,spawn_entity(game,EntityKind::Bear,bank.stand))) {
            start_bear_fishing(*bear);bear->facing=bank.fish-bank.stand;
            placed.push_back(bank.stand);result="Working bear and finite fish";
        } else result="Fish placed; no capacity for bear";
    }
    component_result(report,roll,result,placed);
}

void approach(Game& game,std::span<const Cell> ground,int parent,Cell anchor,GenerationReport* report) {
    std::vector<Cell> sites,occupied,centers;
    std::vector<Cell> bears;for(const auto& actor:game.entities)if(actor.kind==EntityKind::Bear)bears.push_back(actor.cell);
    for(Cell c:ground)if(vacant(game,c) && distance(c,game.run.spawn)>=14 &&
        std::none_of(bears.begin(),bears.end(),[&](Cell b){return distance(c,b)<10;}))sites.push_back(c);
    for(std::size_t n=sites.size();n>1;--n)std::swap(sites[n-1],sites[random_u32(game)%n]);
    const int stage=biome_stage(game.run.floor);
    const WeightedComponent spreads[]{{2,"Two approach packs",5},{3,"Three approach packs",stage>=3 ? 3U : 0U}};
    const auto spread=roll_component(game,report,feature,parent,"Active den approach",anchor,spreads);
    for(Cell center:sites) {
        if(static_cast<int>(centers.size())>=spread.value)break;
        if(!vacant(game,center) || std::any_of(centers.begin(),centers.end(),[&](Cell c){return distance(c,center)<6;}))continue;
        std::vector<Cell> local;
        for(Cell c:sites)if(vacant(game,c) && distance(c,center)<=4 && clear_attack_sight(game,center,c) &&
            std::none_of(occupied.begin(),occupied.end(),[&](Cell old){return distance(c,old)<2;}))local.push_back(c);
        if(local.size()<6)continue;
        centers.push_back(center);
        const WeightedComponent families[]{{0,"Bats under the roots",4},{1,"Wandering dead",3},{2,"Bank snakes",stage>=2 ? 4U : 0U}};
        const auto family=roll_component(game,report,feature,spread.record,"Approach pack",center,families);
        const WeightedComponent sizes[]{{3,"Three fighters",5},{5,"Five fighters",stage>=2 ? 4U : 1U}};
        const auto size=roll_component(game,report,feature,family.record,"Pack size",center,sizes);
        std::vector<Cell> placed;
        const auto kind=family.value==0 ? EntityKind::Bat : family.value==1 ? EntityKind::Zombie : EntityKind::Snake;
        for(Cell c:local) {
            if(static_cast<int>(placed.size())>=size.value)break;
            if(std::any_of(occupied.begin(),occupied.end(),[&](Cell old){return distance(c,old)<2;}))continue;
            if(get_entity(game,spawn_entity(game,kind,c))){placed.push_back(c);occupied.push_back(c);}
        }
        component_result(report,size,placed.size()==static_cast<std::size_t>(size.value) ? "Active fighters placed" : "Reduced by spacing or entity capacity",placed);
        component_result(report,family,"Direct combat outside the sleeping/fishing bears' buffer",placed);
    }
    component_result(report,spread,centers.size()==static_cast<std::size_t>(spread.value) ? "Separate approach packs" : "Reduced by bear buffers, arrival distance or safe space",centers);
}

void dressing(Game& game,std::span<const Cell> sites,Cell cache,int parent,GenerationReport* report) {
    auto shuffled=std::vector<Cell>(sites.begin(),sites.end());
    for(std::size_t n=shuffled.size();n>1;--n)std::swap(shuffled[n-1],shuffled[random_u32(game)%n]);
    std::vector<Cell> anchors;
    for(Cell anchor:shuffled) {
        if(anchors.size()>=8)break;
        if(!vacant(game,anchor) || distance(anchor,cache)<3 ||
            std::any_of(anchors.begin(),anchors.end(),[&](Cell c){return distance(c,anchor)<6;}))continue;
        anchors.push_back(anchor);
        const WeightedComponent patches[]{{0,"Bare ground",2},{1,"Gnawed remains",4},{2,"Leaf litter",3},{3,"Rotting wood and fungus",3}};
        const auto patch=roll_component(game,report,feature,parent,"Den floor patch",anchor,patches);
        if(!patch.value){component_result(report,patch,"Undisturbed ground");continue;}
        std::vector<Cell> placed;
        for(Cell c:shuffled)if(distance(c,anchor)<=3 && distance(c,cache)>=3 && vacant(game,c) && random_u32(game)%3==0) {
            const auto prop=patch.value==1 ? PropKind::BonePile : patch.value==2 ? (random_u32(game)%2 ? PropKind::Leaves : PropKind::Fern) :
                (random_u32(game)%3 ? PropKind::Puffball : PropKind::RottenLog);
            if(place_prop(game.stage,c,prop,static_cast<std::uint8_t>(random_u32(game))))placed.push_back(c);
        }
        component_result(report,patch,"Scattered low props; preserve beds, residents and loot",placed);
    }
}
}

void populate_forest_den(Game& game,const FloorPlan& plan,GenerationReport* report) {
    for(const auto& den:plan.forest_dens) {
        auto a=ground(game,plan.rooms[static_cast<std::size_t>(den.a)]);
        auto b=ground(game,plan.rooms[static_cast<std::size_t>(den.b)]);
        auto all=a;all.insert(all.end(),b.begin(),b.end());
        const auto group=residents(game,den,report);
        stream(game,a,group.record,den.spring,report);
        approach(game,a,group.record,plan.rooms[static_cast<std::size_t>(den.a)].center,report);
        compose_landmark_supplies(game,report,feature,all,den.cache);
        dressing(game,all,den.cache,group.record,report);
        if(game.run.roof_light_count<static_cast<int>(game.run.roof_lights.size()))
            game.run.roof_lights[static_cast<std::size_t>(game.run.roof_light_count++)]=
                {plan.rooms[static_cast<std::size_t>(den.a)].center,{7,1250,{220,218,164}}};
    }
}
