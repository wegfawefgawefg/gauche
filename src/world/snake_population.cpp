#include "snake_tunnel.hpp"
#include "landmark_supplies.hpp"
#include "components.hpp"
#include "../entities/attacks.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::SnakeTunnel;
bool vacant(const Game& game,const SnakeTunnel& tunnel,Cell cell) {
    const auto* tile=game.stage.at(cell);
    return tile && walkable(*tile) && !prop_blocks(tile->prop) && entity_at(game,cell,false)<0 &&
        distance(cell,tunnel.entry)>3 && distance(cell,tunnel.cache)>2 &&
        (!tunnel.crossing_length || (distance(cell,tunnel.island)>5 && distance(cell,tunnel.tree)>2));
}
void group(Game& game,const SnakeTunnel& tunnel,Cell anchor,GenerationReport* report,std::vector<Cell>& occupied) {
    const WeightedComponent sizes[]{{3,"Three snakes",3},{4,"Four snakes",4},{6,"Six snakes",biome_stage(game.run.floor)>=2 ? 3U : 1U}};
    const auto count=roll_component(game,report,feature,tunnel.component,"Snake colony",anchor,sizes);
    std::vector<Cell> sites;
    for(Cell c:tunnel.ground)if(distance(c,anchor)<=7 && vacant(game,tunnel,c) && clear_attack_sight(game,anchor,c))sites.push_back(c);
    for(std::size_t n=sites.size();n>1;--n)std::swap(sites[n-1],sites[random_u32(game)%n]);
    std::vector<Cell> placed;
    for(Cell c:sites) {
        if(static_cast<int>(placed.size())>=count.value)break;
        if(std::any_of(occupied.begin(),occupied.end(),[&](Cell old){return distance(old,c)<2;}))continue;
        if(auto* snake=get_entity(game,spawn_entity(game,EntityKind::Snake,c))) {
            snake->timer_a=20+static_cast<int>(random_u32(game)%60);
            game.stage.at(c)->prop={};placed.push_back(c);occupied.push_back(c);
        }
    }
    component_result(report,count,placed.size()==static_cast<std::size_t>(count.value) ? "Snakes placed on connected banks" : "Reduced by actual bank space or capacity",placed);
    const WeightedComponent flyers[]{{0,"No flyers",5},{1,"One bat",3},{2,"Two bats",biome_stage(game.run.floor)>=2 ? 2U : 0U}};
    const auto flight=roll_component(game,report,feature,count.record,"Overhead hunters",anchor,flyers);
    std::vector<Cell> bats;
    for(Cell c:sites) {
        if(static_cast<int>(bats.size())>=flight.value)break;
        if(!vacant(game,tunnel,c))continue;
        if(get_entity(game,spawn_entity(game,EntityKind::Bat,c))){game.stage.at(c)->prop={};bats.push_back(c);}
    }
    component_result(report,flight,!flight.value ? "No added flyers" : bats.empty() ? "No vacant site/capacity" : "Additional flying hunters",bats);
    const WeightedComponent scenery[]{{0,"Low vegetation",4},{1,"Scattered bones",4},{2,"Luminous bank",3}};
    const auto dressing=roll_component(game,report,feature,count.record,"Bank scenery",anchor,scenery);
    std::vector<Cell> covered;
    for(Cell c:sites)if(vacant(game,tunnel,c) && game.stage.at(c)->prop.kind==PropKind::None && random_u32(game)%4==0) {
        const auto prop=dressing.value==1 ? (random_u32(game)%3 ? PropKind::BonePile : PropKind::Leaves) :
            dressing.value==2 && covered.empty() ? PropKind::LanternPlant : random_u32(game)%2 ? PropKind::Fern : PropKind::Leaves;
        if(place_prop(game.stage,c,prop,static_cast<std::uint8_t>(random_u32(game)%3)))covered.push_back(c);
    }
    component_result(report,dressing,"Low bank props; gaps and narrow routes remain readable",covered);
}
}

void populate_snake_tunnel(Game& game,const FloorPlan& plan,GenerationReport* report) {
    for(const auto& tunnel:plan.snake_tunnels) {
        auto sites=tunnel.ground;
        for(std::size_t n=sites.size();n>1;--n)std::swap(sites[n-1],sites[random_u32(game)%n]);
        const WeightedComponent densities[]{{0,"Scattered colonies",3},{1,"Busy banks",5},{2,"Crowded banks",biome_stage(game.run.floor)>=2 ? 3U : 0U}};
        const auto density=roll_component(game,report,feature,tunnel.component,"Colony spread",tunnel.entry,densities);
        const int wanted=static_cast<int>(tunnel.rooms.size())+density.value+1;
        std::vector<Cell> anchors,occupied;
        for(Cell c:sites) {
            if(static_cast<int>(anchors.size())>=wanted)break;
            if(!vacant(game,tunnel,c) || std::any_of(anchors.begin(),anchors.end(),[&](Cell old){return distance(old,c)<8;}))continue;
            int space=0;for(Cell other:sites)if(distance(c,other)<=6 && vacant(game,tunnel,other) && clear_attack_sight(game,c,other))++space;
            if(space<8)continue;
            anchors.push_back(c);group(game,tunnel,c,report,occupied);
        }
        component_result(report,density,"Independent snake colonies on actual bank geometry",anchors);
        compose_landmark_supplies(game,report,feature,tunnel.ground,tunnel.cache);
        populate_snake_reward(game,tunnel,report);
    }
}
