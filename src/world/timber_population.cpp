#include "timber_grove.hpp"
#include "landmark_supplies.hpp"
#include "components.hpp"
#include "../entities/attacks.hpp"
#include "../entities/forest_spider.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::TimberGrove;
bool usable(const Game& game,const TimberGrove& grove,Cell cell) {
    const auto* tile=game.stage.at(cell);
    return tile && tile->kind==TileKind::Grass && walkable(*tile) && !prop_blocks(tile->prop) &&
        distance(cell,grove.entry)>=7 && distance(cell,grove.cache)>=3 && entity_at(game,cell,false)<0;
}
void fighters(Game& game,const TimberGrove& grove,Cell anchor,GenerationReport* report,std::vector<Cell>& occupied) {
    const int stage=biome_stage(game.run.floor);
    const WeightedComponent families[]{{0,"Winged pests",4},{1,"Lost dead",3},{2,"Hunting pack",stage>=2 ? 5U : 0U},{3,"Spider hunters",stage>=2 ? 3U : 0U}};
    const auto family=roll_component(game,report,feature,grove.component,"Grove combat pocket",anchor,families);
    const WeightedComponent sizes[]{{3,"Three fighters",stage==1 ? 5U : 2U},{4,"Four fighters",4},{5,"Five fighters",stage>=2 ? 4U : 1U}};
    const auto size=roll_component(game,report,feature,family.record,"Grove pack size",anchor,sizes);
    std::vector<Cell> placed;
    for(int i=0;i<size.value;++i) {
        const EntityKind a=family.value==0 ? EntityKind::Bat : family.value==1 ? EntityKind::Zombie : family.value==2 ? EntityKind::Wolf : EntityKind::ForestSpider;
        const EntityKind b=family.value==0 ? EntityKind::Mosquito : family.value==2 ? EntityKind::Boar : EntityKind::Bat;
        const WeightedComponent members[]{{static_cast<int>(a),family.value==0 ? "Bat" : family.value==1 ? "Zombie" : family.value==2 ? "Wolf" : "Forest spider",7},
            {static_cast<int>(b),family.value==0 ? "Mosquito" : family.value==2 ? "Boar" : "Bat",3}};
        const auto roll=roll_component(game,report,feature,size.record,"Grove fighter",anchor,members);
        std::vector<Cell> candidates;
        for(Cell cell:grove.ground)if(distance(cell,anchor)<=6 && usable(game,grove,cell) && clear_attack_sight(game,anchor,cell) &&
            std::none_of(occupied.begin(),occupied.end(),[&](Cell old){return distance(cell,old)<2;}))candidates.push_back(cell);
        if(candidates.empty()){component_result(report,roll,"No spaced site in this glade");continue;}
        const Cell cell=candidates[random_u32(game)%candidates.size()];
        auto* actor=get_entity(game,spawn_entity(game,static_cast<EntityKind>(roll.value),cell));
        if(!actor){component_result(report,roll,"Entity capacity exhausted");continue;}
        if(actor->kind==EntityKind::ForestSpider)set_forest_spider_role(*actor,random_u32(game)%3==0 ? SpiderYoung : SpiderAdult);
        game.stage.at(cell)->prop={};occupied.push_back(cell);placed.push_back(cell);
        component_result(report,roll,"Fighter placed",std::array{cell});
    }
    component_result(report,size,placed.size()==static_cast<std::size_t>(size.value) ? "Requested fighters placed" : "Reduced by space or capacity",placed);
    component_result(report,family,"Direct fighters; quiet visitors have a separate allowance",placed);
    const WeightedComponent visitors[]{{0,"No visitor",4},{1,"Forager",3},{2,"Woodpecker",2},{3,"Lantern moth",2}};
    const auto visitor=roll_component(game,report,feature,family.record,"Grove visitor",anchor,visitors);
    auto site=std::find_if(grove.ground.begin(),grove.ground.end(),[&](Cell cell){return distance(cell,anchor)<=5 && usable(game,grove,cell);});
    if(!visitor.value)component_result(report,visitor,"Empty slot");
    else if(site==grove.ground.end())component_result(report,visitor,"No visitor site");
    else {
        const auto kind=visitor.value==1 ? EntityKind::ForagerGoblin : visitor.value==2 ? EntityKind::Woodpecker : EntityKind::LanternMoth;
        if(get_entity(game,spawn_entity(game,kind,*site))){game.stage.at(*site)->prop={};component_result(report,visitor,"Additional inhabitant",std::array{*site});}
        else component_result(report,visitor,"Entity capacity exhausted");
    }
}
}

void populate_timber_grove(Game& game,const FloorPlan& plan,GenerationReport* report) {
    for(const auto& grove:plan.timber_groves) {
        if(grove.ground.empty())continue; // A rejected carving returns its rooms to ordinary population.
        auto ground=grove.ground;
        for(std::size_t n=ground.size();n>1;--n)std::swap(ground[n-1],ground[random_u32(game)%n]);
        const WeightedComponent counts[]{{5,"Five pockets",3},{6,"Six pockets",5},{7,"Seven pockets",biome_stage(game.run.floor)>=2 ? 3U : 1U}};
        const auto count=roll_component(game,report,feature,grove.component,"Grove combat spread",grove.center,counts);
        std::vector<Cell> occupied,anchors;
        for(Cell cell:ground) {
            if(static_cast<int>(anchors.size())>=count.value)break;
            if(!usable(game,grove,cell) || std::any_of(anchors.begin(),anchors.end(),[&](Cell old){return distance(cell,old)<9;}))continue;
            int space=0;for(Cell other:ground)if(distance(cell,other)<=5 && usable(game,grove,other) && clear_attack_sight(game,cell,other))++space;
            if(space<16)continue;
            anchors.push_back(cell);fighters(game,grove,cell,report,occupied);
        }
        component_result(report,count,"Combat pockets placed on actual open ground",anchors);
        compose_landmark_supplies(game,report,feature,grove.ground,grove.cache);
        const WeightedComponent lamps[]{{0,"No lantern growth",2},{1,"Lantern growth",3}};
        for(Cell anchor:anchors) {
            const auto lamp=roll_component(game,report,feature,grove.component,"Grove light",anchor,lamps);
            if(!lamp.value){component_result(report,lamp,"No added light");continue;}
            auto site=std::find_if(ground.begin(),ground.end(),[&](Cell cell){return distance(cell,anchor)<=3 && usable(game,grove,cell);});
            if(site==ground.end()){component_result(report,lamp,"No safe light site");continue;}
            game.stage.at(*site)->prop={};place_prop(game.stage,*site,PropKind::LanternPlant);
            component_result(report,lamp,"Real breakable light",std::array{*site});
        }
    }
}
