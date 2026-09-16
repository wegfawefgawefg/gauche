#include "snake_tunnel.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "growth_carving.hpp"
#include "raster.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr auto feature=GenerationFeature::SnakeTunnel;
Cell scale(Cell c,int n){return {c.x*n,c.y*n};}
bool connected(const Game& game,const FloorPlan& plan,std::span<const std::uint8_t> before) {
    if(!generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan))return false;
    const auto after=generation_walking_routes(game,plan);
    for(const auto& room:plan.rooms) {
        const auto i=static_cast<std::size_t>(room.center.y*plan.width+room.center.x);
        if(before[i] && !after[i])return false;
    }
    return true;
}
void ribbon(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,std::span<const Cell> points,int width,std::vector<Cell>& changed) {
    for(std::size_t n=1;n<points.size();++n)for(Cell c:raster_line(points[n-1],points[n],width,plan.width,plan.height).cells)
        if(paint_snake_cell(game,plan,c,TileKind::Grass)){changed.push_back(c);tunnel.ground.push_back(c);}
}
void pit(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,Cell anchor,int parent,GenerationTrace* trace) {
    const WeightedComponent sizes[]{{0,"Solid bank",3},{1,"Small hole",4},{2,"Wide hole",3},{3,"Open pit",biome_stage(game.run.floor)>=2 ? 2U : 0U}};
    const auto roll=roll_component(game,&plan.report,feature,parent,"Bank depression",anchor,sizes);
    const GenerationStep step{trace,game,plan,"Snake bank depression",feature,roll.record};
    if(!roll.value){component_area(&plan.report,roll,"Unbroken bank");return;}
    const auto saved=game.stage.tiles;const auto protected_cells=plan.protected_cells;
    const auto routes=generation_walking_routes(game,plan);
    const int rx=roll.value,ry=1+static_cast<int>(random_u32(game)%static_cast<unsigned>(roll.value));
    const std::array polygon{anchor+Cell{-rx,-ry+1},anchor+Cell{0,-ry},anchor+Cell{rx+1,-ry+1},anchor+Cell{rx,ry+1},anchor+Cell{-rx,ry}};
    if(roll.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(roll.record)];row.guide.assign(polygon.begin(),polygon.end());row.guide_closed=true;}
    std::vector<Cell> changed;
    for(Cell c:raster_polygon(polygon,plan.width,plan.height).cells) {
        const auto i=static_cast<std::size_t>(c.y*plan.width+c.x);
        if(routes[i] || plan.protected_cell(c) || !walkable(game.stage.at_or_border(c)) || distance(c,tunnel.entry)<=3 || distance(c,tunnel.cache)<=3)continue;
        if(paint_snake_cell(game,plan,c,TileKind::Chasm))changed.push_back(c);
    }
    if(!connected(game,plan,routes)) {
        game.stage.tiles=saved;plan.protected_cells=protected_cells;component_area(&plan.report,roll,"Rolled back: bank route would disconnect");
    } else component_area(&plan.report,roll,changed.empty() ? "No unreserved bank fit" : "Real fall hazard; required bank routes preserved",changed);
}
void link(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,Cell a,Cell b,GenerationTrace* trace) {
    const Cell axis=cardinal_toward(a,b,{1,0}),side{-axis.y,axis.x};const int length=distance(a,b);
    const WeightedComponent modes[]{{0,"Rift banks",tunnel.shape==0 ? 8U : 1U},{1,"Braided crossing",tunnel.shape==1 ? 8U : 1U},{2,"Broken floor",tunnel.shape==2 ? 8U : 1U}};
    const auto mode=roll_component(game,&plan.report,feature,tunnel.component,"Snake link",a,modes);
    const GenerationStep step{trace,game,plan,"Snake link",feature,mode.record};
    const auto tiles=game.stage.tiles;const auto protected_cells=plan.protected_cells;
    const auto routes=generation_walking_routes(game,plan);const auto ground_count=tunnel.ground.size();
    const int left=4+static_cast<int>(random_u32(game)%4),right=4+static_cast<int>(random_u32(game)%4);
    const int waist=3+static_cast<int>(random_u32(game)%5);
    const std::array polygon{a+scale(axis,3)-scale(side,left),a+scale(axis,length/2)-scale(side,waist),b-scale(axis,3)-scale(side,right),
        b-scale(axis,3)+scale(side,left),a+scale(axis,length/2)+scale(side,waist),a+scale(axis,3)+scale(side,right)};
    if(mode.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(mode.record)];row.guide.assign(polygon.begin(),polygon.end());row.guide_closed=true;}
    std::vector<Cell> changed;
    for(Cell c:raster_polygon(polygon,plan.width,plan.height).cells) {
        const Cell delta=c-a;const int along=delta.x*axis.x+delta.y*axis.y,across=delta.x*side.x+delta.y*side.y;
        // This link replaces its own central corridor. Other route sockets remain.
        if(plan.protected_cell(c) && (along<5 || along>length-5 || std::abs(across)>2))continue;
        if(paint_snake_cell(game,plan,c,mode.value==2 ? TileKind::Grass : TileKind::Chasm)) {
            changed.push_back(c);if(mode.value==2)tunnel.ground.push_back(c);
        }
    }
    for(int flank:{-1,1}) {
        const int depth=mode.value==0 ? std::max({left,right,waist})+1 : mode.value==1 ? 2+static_cast<int>(random_u32(game)%3) : 0;
        const int bend=mode.value==1 ? static_cast<int>(random_u32(game)%5)-2 : 0;
        const std::array guide{a,a+scale(axis,5)+scale(side,flank*depth),a+scale(axis,length/2)+scale(side,flank*depth+bend),b-scale(axis,5)+scale(side,flank*depth),b};
        const WeightedComponent widths[]{{0,"Single-file route",biome_stage(game.run.floor)>=2 && mode.value==1 ? 3U : 0U},{1,"Broad route",5}};
        const auto route=roll_component(game,&plan.report,feature,mode.record,"Bank route",a,widths);
        std::vector<Cell> cells;ribbon(game,plan,tunnel,guide,route.value,cells);changed.insert(changed.end(),cells.begin(),cells.end());
        if(route.record>=0)plan.report.components[static_cast<std::size_t>(route.record)].guide.assign(guide.begin(),guide.end());
        component_area(&plan.report,route,"Dry route through the link",cells);
    }
    if(mode.value!=2) {
        const WeightedComponent counts[]{{1,"One cross-link",4},{2,"Two cross-links",3},{3,"Three cross-links",1}};
        const auto count=roll_component(game,&plan.report,feature,mode.record,"Cross-links",a,counts);
        std::vector<Cell> cells;
        for(int n=0;n<count.value;++n) {
            const Cell center=a+scale(axis,5+static_cast<int>(random_u32(game)%static_cast<unsigned>(length-9)));
            const int reach=mode.value==0 ? std::max({left,right,waist})+1 : 4;
            const std::array guide{center-scale(side,reach),center+scale(side,reach)};
            ribbon(game,plan,tunnel,guide,0,cells);
        }
        changed.insert(changed.end(),cells.begin(),cells.end());component_area(&plan.report,count,"Dry transverse crossings",cells);
    }
    if(!connected(game,plan,routes)) {
        game.stage.tiles=tiles;plan.protected_cells=protected_cells;tunnel.ground.resize(ground_count);
        component_area(&plan.report,mode,"Rolled back: required room route or exit lock changed");
        if(mode.record>=0)for(auto& row:plan.report.components)if(row.parent==mode.record){row.result="Parent link rolled back";row.cells.clear();row.placed=0;}
        return;
    }
    component_area(&plan.report,mode,"Link carved with connected dry routes",changed);
    if(mode.value==2) {
        // The original required sockets remain dry; the rest of this broad link
        // may break into independently sized holes instead of one giant rift.
        for(Cell c:changed)if(!protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)])plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=0;
        for(int n=0;n<4;++n)pit(game,plan,tunnel,a+scale(axis,5+static_cast<int>(random_u32(game)%static_cast<unsigned>(length-9)))+scale(side,static_cast<int>(random_u32(game)%11)-5),mode.record,trace);
        for(Cell c:changed)plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
    }
}
}

void compose_snake_terrain(Game& game,FloorPlan& plan,SnakeTunnel& tunnel,GenerationTrace* trace) {
    for(std::size_t n=1;n<tunnel.rooms.size();++n)
        link(game,plan,tunnel,plan.rooms[static_cast<std::size_t>(tunnel.rooms[n-1])].center,plan.rooms[static_cast<std::size_t>(tunnel.rooms[n])].center,trace);
    for(int index:tunnel.rooms) {
        const auto& room=plan.rooms[static_cast<std::size_t>(index)];
        const WeightedComponent counts[]{{0,"Unbroken bank",1},{2,"Scattered holes",4},{4,"Pitted bank",tunnel.shape==2 ? 7U : 3U}};
        const auto count=roll_component(game,&plan.report,feature,tunnel.component,"Bank erosion",room.center,counts);
        std::vector<Cell> candidates;
        for(int y=-7;y<=7;++y)for(int x=-7;x<=7;++x) {
            const Cell cell=room.center+Cell{x,y};if(walkable(game.stage.at_or_border(cell)) && !plan.protected_cell(cell))candidates.push_back(cell);
        }
        for(int n=0;n<count.value && !candidates.empty();++n) {
            const Cell anchor=candidates[random_u32(game)%candidates.size()];pit(game,plan,tunnel,anchor,count.record,trace);
        }
        component_area(&plan.report,count,count.value ? "Bank depression rolls resolved below" : "No bank erosion");
    }
}
