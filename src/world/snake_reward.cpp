#include "snake_tunnel.hpp"
#include "landmark_supplies.hpp"
#include "generation_trace.hpp"
#include "growth_carving.hpp"
#include "../props/interaction.hpp"
#include "../props/tall_tree.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr auto feature=GenerationFeature::SnakeTunnel;
Cell scale(Cell c,int n){return {c.x*n,c.y*n};}
bool vacant(const Game& game,Cell c){const auto* tile=game.stage.at(c);return tile && walkable(*tile) && tile->prop.kind==PropKind::None && entity_at(game,c,false)<0;}
bool safe_tree(Game& game,Cell root,Cell dir,int gap) {
    int facing=0;
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    for (int i=0;i<4;++i) if (directions[i]==dir) facing=i;
    if (tree_bridge_length(game,root,dir)!=gap || !vacant(game,root) ||
        !place_prop(game.stage,root,PropKind::TallTree,static_cast<std::uint8_t>(facing))) return false;
    bool valid=floor_reachable(game);
    for (Cell direction:directions) {
        const int length=tree_bridge_length(game,root,direction);if (!length) continue;
        std::array<Tile,3> saved{};
        for (int i=0;i<length;++i) saved[static_cast<std::size_t>(i)]=*game.stage.at(root+scale(direction,i+1));
        lay_tree_bridge(game,root,direction);
        const auto prop=game.stage.at(root)->prop;game.stage.at(root)->prop={};
        valid=valid && floor_lock_required(game);
        game.stage.at(root)->prop=prop;
        for (int i=0;i<length;++i) *game.stage.at(root+scale(direction,i+1))=saved[static_cast<std::size_t>(i)];
    }
    if (!valid) game.stage.at(root)->prop={};
    return valid;
}
}

void carve_snake_reward(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,GenerationTrace* trace) {
    const WeightedComponent rewards[]{{0,"No detached island",3},{1,"Detached cache island",5}};
    const auto reward=roll_component(game,&plan.report,feature,tunnel.component,"Optional bank reward",tunnel.cache,rewards);
    const GenerationStep step{trace,game,plan,"Snake cache island",feature,reward.record};
    tunnel.prize_component=reward.record;
    if(!reward.value){component_result(&plan.report,reward,"No island");return;}
    const WeightedComponent gaps[]{{1,"One-cell gap",3},{2,"Two-cell gap",4},{3,"Three-cell gap",3}};
    const auto width=roll_component(game,&plan.report,feature,reward.record,"Moat width",tunnel.cache,gaps);
    const auto routes=generation_walking_routes(game,plan);auto rooms=tunnel.rooms;
    for(std::size_t n=rooms.size();n>1;--n)std::swap(rooms[n-1],rooms[random_u32(game)%n]);
    const Cell side{-tunnel.axis.y,tunnel.axis.x};const int first=random_u32(game)%2 ? 1 : -1;
    const bool round=random_u32(game)%2!=0;
    for(int index:rooms)for(int gap=width.value;gap>=1;--gap)for(int flank:{first,-first}) {
        const Cell bank=plan.rooms[static_cast<std::size_t>(index)].center;
        const int extent=gap+3,radius=gap+1;
        const Cell island=bank+scale(tunnel.axis,extent)+scale(side,flank*extent);
        std::vector<Cell> footprint;
        bool clear=true;
        for(int y=-radius;y<=radius;++y)for(int x=-radius;x<=radius;++x) {
            if(round && x*x+y*y>radius*radius+radius)continue;
            const Cell c=island+Cell{x,y};
            if(c.x<=0 || c.y<=0 || c.x>=plan.width-1 || c.y>=plan.height-1 || plan.protected_cell(c) || routes[static_cast<std::size_t>(c.y*plan.width+c.x)])clear=false;
            footprint.push_back(c);
        }
        if(!clear)continue;
        const auto saved=game.stage.tiles;const auto protection=plan.protected_cells;
        for(Cell c:footprint) {
            const Cell d=c-island;
            clear=paint_snake_cell(game,plan,c,std::max(std::abs(d.x),std::abs(d.y))<=1 ? TileKind::Grass : TileKind::Chasm) && clear;
        }
        for(int i=0;i<=extent;++i)for(int j=-1;j<=1;++j)
            clear=paint_snake_cell(game,plan,bank+scale(side,flank*i)+scale(tunnel.axis,j),TileKind::Grass) && clear;
        if(!clear || !generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
            game.stage.tiles=saved;plan.protected_cells=protection;continue;
        }
        tunnel.island=island;tunnel.crossing_length=gap;tunnel.tree=island-scale(tunnel.axis,gap+2);
        component_area(&plan.report,reward,"Optional isolated reward; gear need not be supplied here",footprint);
        const auto result=std::to_string(gap)+"-cell moat fitted against route approaches";
        component_result(&plan.report,width,result.c_str(),std::array{island});return;
    }
    component_result(&plan.report,reward,"No isolated island fits clear of routes and earlier habitats");
    component_result(&plan.report,width,"No moat placed");
}

void populate_snake_reward(Game& game,const SnakeTunnel& tunnel,GenerationReport* report) {
    if(tunnel.crossing_length<=0)return;
    const WeightedComponent trees[]{{0,"No nearby bridge tree",5},{1,"Fellable bank tree",3}};
    const auto tree=roll_component(game,report,feature,tunnel.prize_component,"Crossing opportunity",tunnel.tree,trees);
    if(!tree.value)component_result(report,tree,"Use traversal gear found elsewhere");
    else if(safe_tree(game,tunnel.tree,tunnel.axis,tunnel.crossing_length))component_result(report,tree,"Real tree bridge opportunity; no supplied axe",std::array{tunnel.tree});
    else component_result(report,tree,"Tree would obstruct a route or bridge the exit lock");
    compose_landmark_supplies(game,report,feature,std::array{tunnel.island},tunnel.island,true);
}
