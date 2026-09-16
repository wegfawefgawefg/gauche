#include "forest_encounters.hpp"
#include "forest_encounter_rules.hpp"
#include "../entities/behavior.hpp"
#include "../entities/attacks.hpp"
#include "../entities/dispatch.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr auto feature=GenerationFeature::ForestEncounters;
constexpr Cell sides[]{{1,0},{-1,0},{0,1},{0,-1}};
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
void specialist(Game& game,const RoomPlan& room,int parent,PopulationReport* population,GenerationReport* report,
    std::span<const WeightedComponent> choices) {
    const int stage=biome_stage(game.run.floor);
    const auto roll=roll_component(game,report,feature,parent,"Specialist",room.center,choices);
    const auto kind=static_cast<EntityKind>(roll.value);
    if (kind==EntityKind::None) {component_result(report,roll,"No additional specialist");return;}
    // Stationary threats need a broad, unoccupied ring so they cannot close a
    // narrow crossing. Species init still owns segmented worms and root links.
    auto sites=room_spaces(game,room,kind);
    const bool stationary=kind==EntityKind::RootTurret || kind==EntityKind::Den ||
        kind==EntityKind::Spawner || kind==EntityKind::WaspNest;
    std::erase_if(sites,[&](Cell c){
        if(!approach(game,c))return true;
        if(stationary)for(int y=-1;y<=1;++y)for(int x=-1;x<=1;++x) {
            const Cell adjacent=c+Cell{x,y};
            if(!walkable(game.stage.at_or_border(adjacent)) || entity_at(game,adjacent,false)>=0)return true;
        }
        return false;
    });
    auto* tally=population ? &population->enemies[static_cast<std::size_t>(kind)] : nullptr;
    if(tally)++tally->attempted;
    Handle handle{};
    if(!sites.empty()) {
        const Cell cell=sites[random_u32(game)%sites.size()];
        handle=kind==EntityKind::BurrowWorm ? spawn_burrow_worm(game,cell) : spawn_entity(game,kind,cell);
    }
    if(tally){if(get_entity(game,handle))++tally->placed;else ++tally->rejected;}
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
void wildlife(Game& game,const RoomPlan& room,int parent,PopulationReport* population,GenerationReport* report,
    std::span<const WeightedComponent> choices) {
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
    int groups=0,total=0;
    for (std::size_t index:rooms) {
        const auto& room=plan.rooms[index];
        const auto rules=forest_encounter_rules(game.run.floor,room.role,plan.themes);
        if(rules.exclusion)continue;
        auto sites=room_spaces(game,room);std::erase_if(sites,[&](Cell c){return !approach(game,c);});
        // Shared budgets used to starve later rooms. A local footprint limits
        // each group; layout and composition rolls vary pressure without that bias.
        const auto shape=roll_component(game,report,feature,-1,"Ordinary encounter",room.center,rules.spacing);
        const auto size=roll_component(game,report,feature,shape.record,"Group size",room.center,rules.sizes);
        const int wanted=std::min(size.value,std::max(1,static_cast<int>(sites.size()/24)));
        // Roll a family first, then independently roll individual members.
        const auto family=roll_component(game,report,feature,shape.record,"Pack family",room.center,rules.families);
        const auto& members=rules.members[static_cast<std::size_t>(family.value)];
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
        specialist(game,room,shape.record,population,report,rules.specialists);
        wildlife(game,room,shape.record,population,report,rules.wildlife);
    }
    decision.variant=std::to_string(groups)+" ordinary groups / "+std::to_string(total)+" direct fighters";
    if (!groups) {decision.outcome=GenerationOutcome::Failed;decision.reason="No eligible ordinary encounter space; landmark populations are independent";}
    if (report) report->features.push_back(std::move(decision));
}
