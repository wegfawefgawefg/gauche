#include "spider_growth.hpp"
#include "components.hpp"
#include "population_report.hpp"
#include "../entities/forest_spider.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
bool vacant(const Game& game,Cell cell) {
    const auto* tile=game.stage.at(cell);
    return tile && walkable(*tile) && !prop_blocks(tile->prop) && entity_at(game,cell,false)<0 && distance(cell,game.run.spawn)>8;
}
bool spider(Game& game,Cell cell,ForestSpiderRole role,PopulationReport* report) {
    if (!vacant(game,cell)) return false;
    auto* count=report ? &report->enemies[static_cast<std::size_t>(EntityKind::ForestSpider)] : nullptr;
    if (count) ++count->attempted;
    auto* entity=get_entity(game,spawn_entity(game,EntityKind::ForestSpider,cell));
    if (!entity) {if (count) ++count->rejected;return false;}
    if (count) ++count->placed;
    set_forest_spider_role(*entity,role);entity->timer_a=30+static_cast<int>(random_u32(game)%61);
    return true;
}
}
void populate_spider_growth(Game& game,FloorPlan& plan,PopulationReport* report) {
    std::vector<Cell> colonies;
    std::vector<bool> claimed(game.stage.tiles.size(),false);
    for (const auto& growth:plan.spider_growth) {
        auto sites=growth.ground;
        for (std::size_t i=sites.size();i>1;--i) std::swap(sites[i-1],sites[random_u32(game)%i]);
        int occupants=0,pockets=0,webs=0;
        const int target=std::clamp(static_cast<int>(sites.size()/45),2,6);
        const WeightedComponent choices[]{{0,"Bare passage",2},{1,"Web bank",3},{2,"Hunting spider",4},{3,"Nursery",game.run.floor==1 ? 1U : 3U},{4,"Discarded prey",2}};
        for (Cell cell:sites) {
            if (pockets>=target) break;
            if (!vacant(game,cell)) continue;
            bool near=false;for (Cell old:colonies) if (distance(cell,old)<5) near=true;
            if (near) continue;
            colonies.push_back(cell);++pockets;
            const auto pocket=roll_component(game,&plan.report,GenerationFeature::SpiderCave,growth.component,"Spread pocket",cell,choices);
            std::vector<Cell> placed;
            if (pocket.value==2 || pocket.value==3) {
                if (spider(game,cell,SpiderAdult,report)) {placed.push_back(cell);++occupants;}
                if (pocket.value==3) {
                    const int young=2+static_cast<int>(random_u32(game)%4);
                    for (Cell site:sites) {
                        if (static_cast<int>(placed.size())>=young+1) break;
                        if (distance(site,cell)>3 || site==cell) continue;
                        if (spider(game,site,SpiderYoung,report)) {placed.push_back(site);++occupants;}
                    }
                }
            }
            if (pocket.value==1 || pocket.value==3 || pocket.value==4) {
                const int radius=2+static_cast<int>(random_u32(game)%3);
                for (Cell site:sites) {
                    auto* tile=game.stage.at(site);
                    if (distance(site,cell)>radius || !tile || !walkable(*tile) || tile->prop.kind!=PropKind::None) continue;
                    if (random_u32(game)%3==0) continue;
                    if (pocket.value==4 && entity_at(game,site,false)>=0) continue;
                    const auto kind=pocket.value==4 ? PropKind::BonePile : PropKind::ForestWeb;
                    place_prop(game.stage,site,kind,static_cast<std::uint8_t>(random_u32(game)%3));
                    if (kind==PropKind::ForestWeb) ++webs;
                    if (std::find(placed.begin(),placed.end(),site)==placed.end()) placed.push_back(site);
                }
            }
            component_result(&plan.report,pocket,pocket.value==0 ? "Empty passage" : placed.empty() ? "No free sites" : "Habitat pocket placed",placed);
        }
        // Sparse silk joins the pockets into a readable territory. Shared cells
        // roll once even when two branches overlap, and existing props survive.
        for (Cell cell:sites) {
            const auto index=static_cast<std::size_t>(cell.y*plan.width+cell.x);
            if (claimed[index]) continue;
            claimed[index]=true;
            auto* tile=game.stage.at(cell);
            if (tile->prop.kind!=PropKind::None || !walkable(*tile) || distance(cell,game.run.spawn)<8) continue;
            if (random_u32(game)%100<12) {place_prop(game.stage,cell,PropKind::ForestWeb);++webs;}
        }
        if (growth.component>=0) plan.report.components[static_cast<std::size_t>(growth.component)].result=
            "Connected habitat; "+std::to_string(growth.opened)+" walls opened; "+std::to_string(occupants)+
            " spiders, "+std::to_string(webs)+" webs, "+std::to_string(pockets)+" pockets";
    }
}
