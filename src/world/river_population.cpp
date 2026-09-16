#include "rivers.hpp"
#include "components.hpp"
#include "population_report.hpp"
#include "water.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

void populate_forest_river(Game& game,FloorPlan& plan,PopulationReport* report) {
    for (const auto& river:plan.rivers) {
        auto sites=river.banks;
        for (std::size_t i=sites.size();i>1;--i) std::swap(sites[i-1],sites[random_u32(game)%i]);
        std::vector<Cell> occupied;
        const int budget=std::clamp(static_cast<int>(river.path.size()/12),3,12);
        const WeightedComponent enemies[]{{static_cast<int>(EntityKind::Mosquito),"Mosquito",3},{static_cast<int>(EntityKind::Bat),"Bat",2},
            {static_cast<int>(EntityKind::Zombie),"Zombie",2},{static_cast<int>(EntityKind::Wolf),"Wolf",game.run.floor==1 ? 0U : 2U}};
        for (Cell c:sites) {
            if (static_cast<int>(occupied.size())>=budget) break;
            const auto* tile=game.stage.at(c);
            if (!tile || !walkable(*tile) || entity_at(game,c,false)>=0 || distance(c,game.run.spawn)<9) continue;
            bool near=false;for (Cell old:occupied) if (distance(old,c)<6) near=true;
            if (near) continue;
            const auto roll=roll_component(game,&plan.report,GenerationFeature::River,river.component,"Riverbank fighter",c,enemies);
            auto* tally=report ? &report->enemies[static_cast<std::size_t>(roll.value)] : nullptr;if (tally) ++tally->attempted;
            if (get_entity(game,spawn_entity(game,static_cast<EntityKind>(roll.value),c))) {
                if (tally) ++tally->placed;
                occupied.push_back(c);component_result(&plan.report,roll,"Bank inhabitant placed",std::array{c});
            } else {if (tally) ++tally->rejected;component_result(&plan.report,roll,"Entity capacity exhausted");}
        }
        int plants=0;
        for (Cell c:sites) {
            auto* tile=game.stage.at(c);
            if (!tile || !walkable(*tile) || tile->prop.kind!=PropKind::None || entity_at(game,c,false)>=0 || random_u32(game)%100>=35) continue;
            place_prop(game.stage,c,random_u32(game)%3 ? PropKind::Fern : PropKind::Puffball,static_cast<std::uint8_t>(random_u32(game)%4));++plants;
        }
        const WeightedComponent shoals[]{{0,"No shoal",3},{1,"Fish shoal",4}};
        const int pockets=std::clamp(static_cast<int>(river.path.size()/20),1,5);
        for (int i=0;i<pockets;++i) {
            const Cell c=river.path[static_cast<std::size_t>((i+1)*static_cast<int>(river.path.size())/(pockets+1))];
            const auto roll=roll_component(game,&plan.report,GenerationFeature::River,river.component,"River shoal",c,shoals);
            if (!roll.value) {component_result(&plan.report,roll,"Empty water");continue;}
            if (!river_water(game.stage.at_or_border(c).kind) || entity_at(game,c,false)>=0) {component_result(&plan.report,roll,"Water occupied or changed");continue;}
            auto* fish=get_entity(game,spawn_entity(game,EntityKind::GroundItem,c));
            if (!fish) {component_result(&plan.report,roll,"Entity capacity exhausted");continue;}
            fish->ground_item=make_item(ItemKind::RiverFish,2+static_cast<int>(random_u32(game)%4));fish->sprite=item_sprite(ItemKind::RiverFish);
            if (report) {auto& tally=report->supplies[static_cast<std::size_t>(ItemKind::RiverFish)];++tally.attempted;++tally.placed;}
            component_result(&plan.report,roll,"Fish available to catch",std::array{c});
        }
        if (river.component>=0) plan.report.components[static_cast<std::size_t>(river.component)].result=
            std::string(river.loop ? "Circulating channel; " : "Connected channel; ")+
            std::to_string(occupied.size())+"/"+std::to_string(budget)+" bank fighters, "+std::to_string(plants)+" plants";
    }
}
