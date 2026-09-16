#include "forest_encounters.hpp"
#include "components.hpp"
#include "../entities/behavior.hpp"
#include "../entities/attacks.hpp"
#include "../entities/dispatch.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr auto feature=GenerationFeature::ForestEncounters;
constexpr Cell sides[]{{1,0},{-1,0},{0,1},{0,-1}};
using Options=std::vector<WeightedComponent>;
void option(Options& options,EntityKind kind,const char* name,unsigned weight) {
    options.push_back({static_cast<int>(kind),name,weight});
}
bool approach(const Game& game,Cell cell) {
    const auto* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || distance(cell,game.run.spawn)<9 || entity_at(game,cell,false)>=0) return false;
    int open=0;
    for (Cell side:sides) {
        const auto* adjacent=game.stage.at(cell+side);
        open+=adjacent && walkable(*adjacent) && entity_at(game,cell+side,true)<0;
    }
    return open>=2;
}
std::vector<Cell> place_group(Game& game,GenerationReport* report,int parent,Cell anchor,
    const std::vector<Cell>& sites,int wanted,bool cluster,std::span<const WeightedComponent> choices,
    PopulationReport* population) {
    std::vector<Cell> placed;
    for (int i=0;i<wanted;++i) {
        const auto roll=roll_component(game,report,feature,parent,"Pack member",anchor,choices);
        const auto kind=static_cast<EntityKind>(roll.value);
        auto* count=population ? &population->enemies[static_cast<std::size_t>(kind)] : nullptr;
        if (count) ++count->attempted;
        std::vector<Cell> eligible;
        for (Cell cell:sites) {
            if (!approach(game,cell) || (cluster && (distance(cell,anchor)>6 || !clear_attack_sight(game,anchor,cell)))) continue;
            bool near=false;for (Cell old:placed) if (distance(cell,old)<2) near=true;
            if (!near) eligible.push_back(cell);
        }
        if (eligible.empty()) {
            if (count) ++count->rejected;
            component_result(report,roll,"No spaced approach cell in this group");continue;
        }
        const Cell cell=eligible[random_u32(game)%eligible.size()];
        if (get_entity(game,spawn_entity(game,kind,cell))) {
            if (count) ++count->placed;
            placed.push_back(cell);component_result(report,roll,"Fighter placed",std::array{cell});
        } else {
            if (count) ++count->rejected;
            component_result(report,roll,"Entity capacity exhausted");
        }
    }
    return placed;
}
void specialist(Game& game,const RoomPlan& room,int parent,PopulationReport* population,GenerationReport* report) {
    const int stage=biome_stage(game.run.floor);
    Options choices;option(choices,EntityKind::None,"No specialist",6);
    // Specialists have their own allowance; passive or conditional inhabitants
    // never spend the ordinary pack's slots. Their existing behavior stays intact.
    if (stage>=2) switch(room.role) {
    case RoomRole::Thicket:
        option(choices,EntityKind::RootTurret,"Root watch",3);
        option(choices,EntityKind::ThornSnail,"Thorn snail",2);
        option(choices,EntityKind::BurrowWorm,"Burrow worm",stage>=3 ? 2U : 0U);break;
    case RoomRole::Den:
        option(choices,EntityKind::Den,"Wolf den",3);
        option(choices,EntityKind::BurrowWorm,"Burrow worm",stage>=3 ? 2U : 0U);break;
    case RoomRole::Ruins:option(choices,EntityKind::ZombieStack,"Zombie stack",stage>=3 ? 4U : 0U);break;
    case RoomRole::Cache:case RoomRole::Workshop:option(choices,EntityKind::CrateMimic,"Mimic",3);break;
    case RoomRole::Shrine:
        option(choices,EntityKind::RootTurret,"Root watch",2);
        option(choices,EntityKind::Bear,"Lone bear",1);break;
    case RoomRole::Orchard:option(choices,EntityKind::WaspNest,"Wasp nest",3);break;
    case RoomRole::Brook:option(choices,EntityKind::SporeToad,"Spore toad",3);break;
    default:option(choices,EntityKind::Owl,"Owl perch",2);break;
    }
    const auto roll=roll_component(game,report,feature,parent,"Specialist",room.center,choices);
    const auto kind=static_cast<EntityKind>(roll.value);
    if (kind==EntityKind::None) {component_result(report,roll,"No additional specialist");return;}
    RoomSupplies allowance{4,0,0,0,0,population};
    // Species init owns its links, especially segmented worms and root guards.
    const Handle handle=spawn_room_enemy(game,room,kind,2,allowance);
    const auto* entity=get_entity(game,handle);
    if (!entity) {component_result(report,roll,"No suitable cell or entity capacity");return;}
    const Cell cell=entity->cell;
    std::vector<Cell> placed{cell};
    if (kind==EntityKind::RootTurret && stage>=3) for (Cell side:sides) {
        const Cell guard_cell=cell+side;
        if (!approach(game,guard_cell)) continue;
        if (auto* guard=get_entity(game,spawn_entity(game,EntityKind::BrambleGuard,guard_cell))) {
            guard->entity_a=handle;placed.push_back(guard_cell);
            if (population) {auto& count=population->enemies[static_cast<std::size_t>(EntityKind::BrambleGuard)];++count.attempted;++count.placed;}
        }
        break;
    }
    component_result(report,roll,"Specialist placed",placed);
}
void wildlife(Game& game,const RoomPlan& room,int parent,PopulationReport* population,GenerationReport* report) {
    Options choices;option(choices,EntityKind::None,"No wildlife",3);
    option(choices,EntityKind::LanternMoth,"Lantern moth",2);
    option(choices,EntityKind::CarrionCrow,"Scavenging crow",room.role==RoomRole::Ruins || room.role==RoomRole::Den ? 3U : 1U);
    if (room.role==RoomRole::Brook) option(choices,EntityKind::Bunny,"Rabbit",4);
    if (room.role==RoomRole::Orchard || room.role==RoomRole::Clearing) option(choices,EntityKind::Chicken,"Chicken family",4);
    if (room.role==RoomRole::Workshop || room.role==RoomRole::Cache) {
        option(choices,EntityKind::ForagerGoblin,"Forager",3);
        option(choices,EntityKind::Dog,"Dog",1);
        option(choices,EntityKind::Woodpecker,"Woodpecker",2);
    }
    const auto roll=roll_component(game,report,feature,parent,"Wildlife / scavengers",room.center,choices);
    const auto kind=static_cast<EntityKind>(roll.value);
    if (kind==EntityKind::None) {component_result(report,roll,"No wildlife");return;}
    RoomSupplies allowance{1,0,0,0,0,population};
    if (kind==EntityKind::Chicken) {
        if (const auto cell=room_space(game,room)) {
            const int before=static_cast<int>(std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::Chicken;}));
            spawn_chicken_family(game,*cell);
            const int after=static_cast<int>(std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::Chicken;}));
            component_result(report,roll,after>before ? "Chicken family placed" : "Entity capacity exhausted",after>before ? std::span<const Cell>(&*cell,1) : std::span<const Cell>{});return;
        }
    } else if (const auto* e=get_entity(game,spawn_room_enemy(game,room,kind,1,allowance))) {
        component_result(report,roll,"Background inhabitant placed",std::array{e->cell});return;
    }
    component_result(report,roll,"No suitable cell or entity capacity");
}
}

void populate_forest_encounters(Game& game,const FloorPlan& plan,std::span<const std::size_t> rooms,
    PopulationReport* population,GenerationReport* report) {
    FeatureDecision decision;decision.feature=feature;
    decision.denominator=feature_denominator(generation_rule(feature),game.run.floor);
    if (!decision.denominator) {
        decision.outcome=GenerationOutcome::Ineligible;decision.reason="Outside Forest";
        if (report) report->features.push_back(std::move(decision));
        return;
    }
    decision.roll=random_u32(game)%decision.denominator;
    decision.candidate_count=static_cast<int>(rooms.size());
    decision.outcome=GenerationOutcome::Built;
    decision.reason="Ordinary fighter packs; wildlife and specialists have separate slots. Landmark inhabitants are independent.";
    const int stage=biome_stage(game.run.floor);
    int groups=0,total=0;
    for (std::size_t index:rooms) {
        const auto& room=plan.rooms[index];
        auto sites=room_spaces(game,room);std::erase_if(sites,[&](Cell c){return !approach(game,c);});
        // Shared budgets used to starve later rooms. A local footprint limits
        // each group; layout and composition rolls vary pressure without that bias.
        const WeightedComponent shapes[]{{0,"Loose inhabitants",3},{1,"Close pack",4},{2,"Two pockets",stage>=2 ? 3U : 1U}};
        const auto shape=roll_component(game,report,feature,-1,"Ordinary encounter",room.center,shapes);
        const WeightedComponent counts[]{{2,"Pair",stage==1 ? 5U : 2U},{3,"Three",4},{4,"Four",stage==1 ? 1U : 4U},{5,"Five",stage>=3 ? 3U : 0U},{6,"Six",stage>=3 ? 1U : 0U}};
        const auto size=roll_component(game,report,feature,shape.record,"Group size",room.center,counts);
        const int wanted=std::min(size.value,std::max(1,static_cast<int>(sites.size()/24)));
        // Roll a family first, then independently roll individual members.
        const bool wet=room.role==RoomRole::Brook || has_theme(plan.themes,GenerationTheme::WetWoods);
        const bool ruins=room.role==RoomRole::Ruins || has_theme(plan.themes,GenerationTheme::Ruins);
        const WeightedComponent families[]{{0,"Winged pests",wet ? 6U : 3U},{1,"Restless dead",ruins ? 7U : 2U},
            {2,"Hunting animals",stage>=2 ? 4U : 0U},{3,"Mixed prowlers",stage>=2 ? 3U : 0U},{4,"Spider hunters",has_theme(plan.themes,GenerationTheme::Spiders) ? 9U : 0U}};
        const auto family=roll_component(game,report,feature,shape.record,"Pack family",room.center,families);
        Options members;
        if (family.value==0) {option(members,EntityKind::Mosquito,"Mosquito",wet ? 5U : 2U);option(members,EntityKind::Bat,"Bat",wet ? 2U : 5U);}
        else if (family.value==1) {option(members,EntityKind::Zombie,"Zombie",8);option(members,EntityKind::Bat,"Bat",2);}
        else if (family.value==2) {option(members,EntityKind::Wolf,"Wolf",5);option(members,EntityKind::Boar,"Boar",2);}
        else if (family.value==4) {option(members,EntityKind::ForestSpider,"Forest spider",8);option(members,EntityKind::Bat,"Bat",2);}
        else {option(members,EntityKind::Zombie,"Zombie",3);option(members,EntityKind::Wolf,"Wolf",3);option(members,EntityKind::Bat,"Bat",2);option(members,EntityKind::Mosquito,"Mosquito",2);}
        std::vector<Cell> placed;
        if (!sites.empty()) {
            const Cell anchor=sites[random_u32(game)%sites.size()];
            placed=place_group(game,report,family.record,anchor,sites,shape.value==2 ? (wanted+1)/2 : wanted,shape.value!=0,members,population);
            if (shape.value==2) {
                auto far=sites;std::erase_if(far,[&](Cell c){return distance(c,anchor)<7 || !approach(game,c);});
                if (!far.empty()) {
                    const Cell second=far[random_u32(game)%far.size()];
                    auto second_sites=sites;
                    std::erase_if(second_sites,[&](Cell c){return std::any_of(placed.begin(),placed.end(),[&](Cell old){return distance(c,old)<2;});});
                    auto others=place_group(game,report,family.record,second,second_sites,wanted/2,true,members,population);
                    placed.insert(placed.end(),others.begin(),others.end());
                }
            }
        }
        component_result(report,size,placed.size()==static_cast<std::size_t>(size.value) ? "Requested size placed" : "Reduced by approach space, footprint or capacity",placed);
        component_result(report,family,placed.empty() ? "No fighter placement" : "Pack members placed",placed);
        component_result(report,shape,placed.empty() ? "No suitable pack space" : "Fighters placed; independent specialist and wildlife rolls follow",placed);
        if (population) {
            auto& count=population->scenes[static_cast<std::size_t>(room.role)];++count.attempted;
            if (placed.empty()) ++count.rejected;else ++count.placed;
        }
        groups+=!placed.empty();total+=static_cast<int>(placed.size());
        decision.regions.push_back({room.center-Cell{room.half_width,room.half_height},room.center+Cell{room.half_width+1,room.half_height+1}});
        specialist(game,room,shape.record,population,report);
        wildlife(game,room,shape.record,population,report);
    }
    decision.variant=std::to_string(groups)+" ordinary groups / "+std::to_string(total)+" direct fighters";
    if (!groups) {decision.outcome=GenerationOutcome::Failed;decision.reason="No eligible ordinary encounter space; landmark populations are independent";}
    if (report) report->features.push_back(std::move(decision));
}
