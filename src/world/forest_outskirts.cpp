#include "forest_outskirts.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "population_report.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <array>

namespace {
constexpr auto feature=GenerationFeature::ForestOutskirts;
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
std::size_t index(const Stage& stage,Cell c) {return static_cast<std::size_t>(c.y*stage.width+c.x);}
// Generation-only walking distance. Doors/actors do not hide the terrain behind
// them; occupancy is checked separately at actual spawn sockets.
std::vector<int> distances(const Stage& stage,std::span<const Cell> starts) {
    std::vector<int> result(stage.tiles.size(),-1);std::vector<Cell> queue;
    for(Cell c:starts)if(stage.in_bounds(c) && result[index(stage,c)]<0){result[index(stage,c)]=0;queue.push_back(c);}
    for(std::size_t i=0;i<queue.size();++i)for(Cell d:sides) {
        const Cell c=queue[i]+d;const auto* tile=stage.at(c);
        if(!tile || !walkable(*tile) || result[index(stage,c)]>=0)continue;
        result[index(stage,c)]=result[index(stage,queue[i])]+1;queue.push_back(c);
    }
    return result;
}
bool threat(const Entity& e) {
    if(e.health<=0)return false;
    switch(e.kind) {
    case EntityKind::Zombie:case EntityKind::ZombieStack:case EntityKind::Bat:
    case EntityKind::Mosquito:case EntityKind::Wolf:case EntityKind::Boar:
    case EntityKind::Snake:case EntityKind::ForestSpider:case EntityKind::Wasp:
    case EntityKind::BurrowWorm:case EntityKind::RootTurret:case EntityKind::BrambleGuard:
    case EntityKind::ThornSnail:case EntityKind::SporeToad:case EntityKind::Owl:
    case EntityKind::Den:case EntityKind::Spawner:case EntityKind::WaspNest:
    case EntityKind::Encounter:case EntityKind::OldGrowthBear:return true;
    default:return false;
    }
}
std::vector<Cell> large_quiet_areas(const Stage& stage,std::span<const std::uint8_t> allowed,
    std::span<const int> nearest) {
    std::vector<bool> seen(stage.tiles.size());std::vector<Cell> result;
    const auto quiet=[&](Cell c){const auto i=index(stage,c);return allowed[i] && (nearest[i]<0 || nearest[i]>8);};
    for(int y=0;y<stage.height;++y)for(int x=0;x<stage.width;++x) {
        const Cell start{x,y};if(seen[index(stage,start)] || !quiet(start))continue;
        std::vector<Cell> group{start};seen[index(stage,start)]=true;
        for(std::size_t i=0;i<group.size();++i)for(Cell d:sides) {
            const Cell c=group[i]+d;if(!stage.in_bounds(c) || seen[index(stage,c)] || !quiet(c))continue;
            seen[index(stage,c)]=true;group.push_back(c);
        }
        if(group.size()>=96)result.insert(result.end(),group.begin(),group.end());
    }
    return result;
}
}

void populate_forest_outskirts(Game& game,FloorPlan& plan,PopulationReport* report,GenerationTrace* trace) {
    if(!roll_generation_feature(game,plan,feature))return;
    const int stage=biome_stage(game.run.floor);
    auto& decision=plan.report.features.back();
    const auto reachable=distances(game.stage,std::array{game.run.spawn});
    std::vector<std::uint8_t> allowed(game.stage.tiles.size()),occupied(allowed.size());
    for(const auto& e:game.entities)if(e.kind!=EntityKind::None && game.stage.in_bounds(e.cell))occupied[index(game.stage,e.cell)]=1;
    for(int y=1;y<plan.height-1;++y)for(int x=1;x<plan.width-1;++x) {
        const Cell c{x,y};const auto i=index(game.stage,c);const auto& tile=game.stage.tiles[i];
        allowed[i]=reachable[i]>=14 && walkable(tile) && tile.kind!=TileKind::Lava && !prop_blocks(tile.prop);
    }
    // Keep authored refuges, sleeping bear scenes, objective rooms and optional
    // islands intact. The new pass owns only ordinary reachable outskirts.
    for(const auto& room:plan.rooms)if(reserved_habitat(room) || room.role==RoomRole::Entrance ||
        room.role==RoomRole::Exit || room.role==RoomRole::Shrine || room.role==RoomRole::Secret)
        for(int y=room.center.y-room.half_height-2;y<=room.center.y+room.half_height+2;++y)
            for(int x=room.center.x-room.half_width-2;x<=room.center.x+room.half_width+2;++x)
                if(game.stage.in_bounds({x,y}))allowed[index(game.stage,{x,y})]=0;
    for(const auto& e:game.entities)if(e.kind==EntityKind::Bear || e.kind==EntityKind::OldGrowthBear)
        for(int y=e.cell.y-12;y<=e.cell.y+12;++y)for(int x=e.cell.x-12;x<=e.cell.x+12;++x)
            if(game.stage.in_bounds({x,y}))allowed[index(game.stage,{x,y})]=0;
    int attempts=0,selections=0,groups=0,bodies=0;decision.candidate_count=0;
    for(int attempt=0;attempt<stage+2;++attempt) {
        std::vector<Cell> threats;for(const auto& e:game.entities)if(threat(e))threats.push_back(e.cell);
        const auto nearest=distances(game.stage,threats);
        auto candidates=large_quiet_areas(game.stage,allowed,nearest);
        const auto free=[&](Cell c){return game.stage.in_bounds(c) && allowed[index(game.stage,c)] &&
            !occupied[index(game.stage,c)] && !plan.protected_cell(c) && game.stage.at_or_border(c).kind!=TileKind::Spring;};
        std::erase_if(candidates,[&](Cell c){int open=0;for(Cell d:sides)open+=free(c+d);return !free(c) || open<3;});
        if(!attempt)decision.candidate_count=static_cast<int>(candidates.size());
        if(candidates.empty())break;
        const Cell anchor=candidates[random_u32(game)%candidates.size()];
        const auto local=distances(game.stage,std::array{anchor});
        std::vector<Cell> sites;
        bool wet=false,wood=false,wide=true;
        for(int y=-1;y<=1;++y)for(int x=-1;x<=1;++x)wide&=free(anchor+Cell{x,y});
        for(int y=anchor.y-6;y<=anchor.y+6;++y)for(int x=anchor.x-6;x<=anchor.x+6;++x) {
            const Cell c{x,y};if(!game.stage.in_bounds(c))continue;const auto i=index(game.stage,c);
            const auto& tile=game.stage.tiles[i];wet|=surface_wet(tile);wood|=tile.material==TileMaterial::Root || tile.material==TileMaterial::Timber || tile.material==TileMaterial::Tree;
            if(local[i]>=0 && local[i]<=6 && free(c) && (nearest[i]<0 || nearest[i]>8))sites.push_back(c);
        }
        const unsigned later=stage>=2 ? 1U : 0U;
        const WeightedComponent choices[]{{0,"Quiet stretch",3},{1,"Pest pocket",3},{2,"Wandering dead",2},
            {3,"Hunting pack",2*later},{4,"Root watch",wide ? (wood ? 4U : 1U)*later : 0U},
            {5,"Thorn snail",2*later},{6,"Spore toad",wet ? 4*later : 0U},
            {7,"Owl perch",2*later},{8,"Wasp nest",wide ? 2*later : 0U},
            {9,"Wolf den",wide && stage>=3 ? 1U : 0U}};
        const auto group=roll_component(game,&plan.report,feature,-1,"Outlying encounter",anchor,choices);
        const GenerationStep checkpoint{trace,game,plan,"Outlying encounter attempt",feature,group.record};
        ++attempts;
        // Consume the rolled patch even when empty; a miss is not rerolled until
        // populated. The untouched remainder of a large sector may still roll.
        for(std::size_t i=0;i<allowed.size();++i)if(local[i]>=0 && local[i]<=10)allowed[i]=0;
        if(!group.value){component_result(&plan.report,group,"Quiet outcome; no actors added");continue;}
        ++selections;
        for(std::size_t i=sites.size();i>1;--i)std::swap(sites[i-1],sites[random_u32(game)%i]);
        const bool specialist=group.value>=4;
        const WeightedComponent sizes[]{{1,"Lone specialist",specialist ? 4U : 0U},
            {2,specialist ? "Root and guard" : "Pair",!specialist || (group.value==4 && stage>=3) ? 4U : 0U},
            {3,"Three",specialist ? 0U : 3U},{4,"Four",!specialist && stage>=3 ? 2U : 0U}};
        const auto size=roll_component(game,&plan.report,feature,group.record,"Outlying group size",anchor,sizes);
        const int wanted=size.value;
        std::vector<Cell> placed;Handle ward{};
        for(int member=0;member<wanted;++member) {
            EntityKind kind=EntityKind::None;
            if(group.value==1)kind=EntityKind::Bat;
            if(group.value==2)kind=EntityKind::Zombie;
            if(group.value==3)kind=EntityKind::Wolf;
            constexpr EntityKind specialists[]{EntityKind::RootTurret,EntityKind::ThornSnail,EntityKind::SporeToad,EntityKind::Owl,EntityKind::WaspNest,EntityKind::Den};
            if(group.value>=4)kind=member ? EntityKind::BrambleGuard : specialists[group.value-4];
            constexpr const char* names[]{"Quiet","Bat","Zombie","Wolf","Root turret","Thorn snail","Spore toad","Owl","Wasp nest","Wolf den"};
            const WeightedComponent members[]{{static_cast<int>(kind),member && specialist ? "Bramble guard" : names[group.value],5},
                {static_cast<int>(EntityKind::Mosquito),"Mosquito",group.value==1 ? 3U : 0U},
                {static_cast<int>(EntityKind::Bat),"Bat",group.value==2 ? 1U : 0U},
                {static_cast<int>(EntityKind::Boar),"Boar",group.value==3 ? 2U : 0U}};
            const auto roll=roll_component(game,&plan.report,feature,group.record,"Outlying member",anchor,members);
            kind=static_cast<EntityKind>(roll.value);
            Cell cell=anchor;bool found=false;
            if(group.value>=4 && !member)found=!occupied[index(game.stage,anchor)];
            else for(Cell c:sites)if(!occupied[index(game.stage,c)] &&
                std::none_of(placed.begin(),placed.end(),[&](Cell old){return distance(c,old)<2;})){cell=c;found=true;break;}
            auto* count=report ? &report->enemies[static_cast<std::size_t>(kind)] : nullptr;if(count)++count->attempted;
            const Handle handle=found ? spawn_entity(game,kind,cell) : Handle{};
            if(auto* e=get_entity(game,handle)) {
                if(count)++count->placed;
                if(kind==EntityKind::BrambleGuard)e->entity_a=ward;else ward=handle;
                occupied[index(game.stage,cell)]=1;placed.push_back(cell);component_result(&plan.report,roll,"Outlying inhabitant placed",std::array{cell});
            } else {if(count)++count->rejected;component_result(&plan.report,roll,found ? "Entity capacity exhausted" : "No spaced socket");}
        }
        component_result(&plan.report,size,placed.size()==static_cast<std::size_t>(wanted) ? "Requested group placed" : "Reduced by available sockets or entity capacity",placed);
        component_result(&plan.report,group,placed.empty() ? "No inhabitants placed" : "Encounter added to reachable quiet ground",placed);
        if(!placed.empty()){++groups;bodies+=static_cast<int>(placed.size());decision.regions.push_back({anchor-Cell{6,6},anchor+Cell{7,7}});}
    }
    decision.outcome=!attempts ? GenerationOutcome::Suppressed : selections && !groups ? GenerationOutcome::Failed : GenerationOutcome::Built;
    decision.variant=std::to_string(groups)+" outlying groups / "+std::to_string(bodies)+" inhabitants";
    decision.reason=(!attempts ? "No eligible large quiet patch; " : selections && !groups ? "All selected encounters failed placement; " : "")+std::to_string(attempts)+" bounded patch rolls; areas need 96 quiet reachable cells, beyond eight walking steps from existing threats. Refuges/objectives excluded.";
}
