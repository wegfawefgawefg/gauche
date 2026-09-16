#include "snake_tunnel.hpp"
#include "components.hpp"
#include "feature_roll.hpp"
#include "../scenery/roof.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
bool eligible(const FloorPlan& plan,int i) {
    return i!=0 && i!=plan.exit_room && i!=plan.objective_room && i!=plan.secret_room &&
        !reserved_habitat(plan.rooms[static_cast<std::size_t>(i)]);
}
}

bool paint_snake_cell(Game& game,FloorPlan& plan,Cell cell,TileKind kind) {
    auto* tile=game.stage.at(cell);
    if(!tile || cell.x<=0 || cell.y<=0 || cell.x>=plan.width-1 || cell.y>=plan.height-1 ||
        tile->break_rule==BreakRule::Unbreakable || tile->material==TileMaterial::Root ||
        tile->contents!=ItemKind::None || tile->prop.kind!=PropKind::None)return false;
    for(const auto& roof:game.stage.roofs)if(roof_covers(roof,cell))return false;
    *tile={kind};plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;return true;
}

void plan_snake_tunnel(Game& game,FloorPlan& plan) {
    if(!roll_generation_feature(game,plan,GenerationFeature::SnakeTunnel))return;
    std::vector<RouteEdge> choices;
    for(auto edge:plan.edges)if(eligible(plan,edge.a) && eligible(plan,edge.b))choices.push_back(edge);
    if(choices.empty()){feature_failed(plan,"No eligible adjacent rooms after earlier habitat reservations");return;}
    plan.report.features.back().candidate_count=static_cast<int>(choices.size());
    auto edge=choices[random_u32(game)%choices.size()];
    if(plan.rooms[static_cast<std::size_t>(edge.a)].depth>plan.rooms[static_cast<std::size_t>(edge.b)].depth)std::swap(edge.a,edge.b);
    SnakeTunnel tunnel;tunnel.rooms={edge.a,edge.b};
    tunnel.axis=plan.rooms[static_cast<std::size_t>(edge.b)].grid-plan.rooms[static_cast<std::size_t>(edge.a)].grid;
    const WeightedComponent forms[]{{0,"Rift banks",4},{1,"Braided crossings",biome_stage(game.run.floor)==1 ? 1U : 4U},{2,"Sinkhole beds",4}};
    const Cell anchor=plan.rooms[static_cast<std::size_t>(edge.a)].center;
    const auto form=roll_component(game,&plan.report,GenerationFeature::SnakeTunnel,-1,"Snake habitat",anchor,forms);
    tunnel.shape=form.value;tunnel.component=form.record;
    std::vector<int> straight,bent;
    for(auto next:plan.edges) {
        const int i=next.a==edge.b ? next.b : next.b==edge.b ? next.a : -1;
        if(i<0 || i==edge.a || !eligible(plan,i))continue;
        const Cell direction=plan.rooms[static_cast<std::size_t>(i)].grid-plan.rooms[static_cast<std::size_t>(edge.b)].grid;
        (direction==tunnel.axis ? straight : bent).push_back(i);
    }
    const WeightedComponent lengths[]{{0,"Two rooms",4},{1,"Straight third room",straight.empty() ? 0U : 3U},{2,"Turning third room",bent.empty() ? 0U : 3U}};
    const auto extent=roll_component(game,&plan.report,GenerationFeature::SnakeTunnel,form.record,"Habitat extent",anchor,lengths);
    if(extent.value) {
        const auto& pool=extent.value==1 ? straight : bent;
        const int last=pool[random_u32(game)%pool.size()];tunnel.rooms.push_back(last);
        tunnel.axis=plan.rooms[static_cast<std::size_t>(last)].grid-plan.rooms[static_cast<std::size_t>(edge.b)].grid;
    }
    std::vector<Cell> centers;
    for(int i:tunnel.rooms) {
        auto& room=plan.rooms[static_cast<std::size_t>(i)];room.shape=RoomShape::SnakeTunnel;room.role=RoomRole::Den;
        room.half_width=7+static_cast<int>(random_u32(game)%3);room.half_height=7+static_cast<int>(random_u32(game)%3);centers.push_back(room.center);
    }
    component_result(&plan.report,extent,"Reserved connected room footprints",centers);
    plan.snake_tunnels.push_back(tunnel);
    feature_reserved(plan,tunnel.rooms,std::string(forms[form.value].name)+" / "+std::to_string(tunnel.rooms.size())+" rooms");
}

void carve_snake_tunnel(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    for(auto& tunnel:plan.snake_tunnels) {
        tunnel.entry=plan.rooms[static_cast<std::size_t>(tunnel.rooms.front())].center;
        const Cell last=plan.rooms[static_cast<std::size_t>(tunnel.rooms.back())].center;
        tunnel.cache=last; // Required reward remains on the connected bank; optional islands are separate.
        compose_snake_terrain(game,plan,tunnel,trace);
        carve_snake_reward(game,plan,tunnel,trace);
        for(int index:tunnel.rooms) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            for(int y=-9;y<=9;++y)for(int x=-9;x<=9;++x) {
                const Cell c=room.center+Cell{x,y};if(!game.stage.in_bounds(c))continue;
                plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
                if(walkable(game.stage.at_or_border(c)))tunnel.ground.push_back(c);
            }
        }
        std::vector<bool> seen(game.stage.tiles.size());
        std::erase_if(tunnel.ground,[&](Cell c){
            const Cell island_delta=c-tunnel.island;
            if(tunnel.crossing_length && std::abs(island_delta.x)<=1 && std::abs(island_delta.y)<=1)return true;
            const auto i=static_cast<std::size_t>(c.y*plan.width+c.x);
            if(seen[i] || !walkable(game.stage.at_or_border(c)))return true;
            seen[i]=true;return false;
        });
        component_area(&plan.report,{tunnel.shape,tunnel.component},"Habitat links and bank erosion composed independently",tunnel.ground);
    }
}
