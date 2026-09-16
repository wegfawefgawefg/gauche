#include "tree_hollow.hpp"
#include "generation_trace.hpp"
#include "growth_carving.hpp"
#include "components.hpp"
#include "raster.hpp"
#include "terrain_material.hpp"
#include "../scenery/hollow_tree.hpp"
#include <algorithm>

namespace {
constexpr auto feature=GenerationFeature::GiantTree;
constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
Cell scale(Cell p,int n){return {p.x*n,p.y*n};}
bool site(const Game& game,const FloorPlan& plan,const GiantTree& tree,Cell cell,std::span<const std::uint8_t> routes) {
    const auto* tile=game.stage.at(cell);
    return tile && tree_ellipse(tree.canopy,cell,4) && !plan.protected_cell(cell) &&
        !routes[static_cast<std::size_t>(cell.y*plan.width+cell.x)] && distance(cell,tree.cache)>2 &&
        tile->contents==ItemKind::None && tile->prop.kind==PropKind::None &&
        (tile->kind==TileKind::Grass || tile->kind==TileKind::Empty);
}
struct Saved {Cell cell;Tile tile;};
void finish(Game& game,FloorPlan& plan,ComponentRoll roll,const std::vector<Saved>& saved,const char* description) {
    if(!generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
        for(const auto& old:saved)*game.stage.at(old.cell)=old.tile;
        component_area(&plan.report,roll,"Rolled back: required route or lock changed");return;
    }
    std::vector<Cell> changed;for(const auto& old:saved)changed.push_back(old.cell);
    component_area(&plan.report,roll,changed.empty() ? "No unreserved interior ground fit this component" : description,changed);
}
void root_partition(Game& game,FloorPlan& plan,const GiantTree& tree,Cell a,Cell bend,Cell b,int width,
    const std::vector<std::uint8_t>& routes,GenerationTrace* trace,bool backbone=false) {
    const WeightedComponent modes[]{{0,"Open lane",backbone ? 0U : 2U},{1,"Broken root ribs",backbone ? 0U : 3U},{2,"Root partition with openings",5}};
    const auto roll=roll_component(game,&plan.report,feature,tree.hollow_component,"Interior root branch",a,modes);
    const GenerationStep step{trace,game,plan,"Hollow root branch",feature,roll.record};
    if(roll.record>=0)plan.report.components[static_cast<std::size_t>(roll.record)].guide={a,bend,b};
    if(!roll.value){component_area(&plan.report,roll,"Open branch; no root wall");return;}
    auto shape=raster_line(a,bend,width,plan.width,plan.height);
    auto next=raster_line(bend,b,width,plan.width,plan.height);shape.cells.insert(shape.cells.end(),next.cells.begin(),next.cells.end());
    std::vector<Cell> doors;
    const int count=1+static_cast<int>(random_u32(game)%3);
    for(int i=0;i<count && !shape.cells.empty();++i)doors.push_back(shape.cells[random_u32(game)%shape.cells.size()]);
    const int opening=1+static_cast<int>(random_u32(game)%2);std::vector<Saved> saved;
    for(Cell cell:shape.cells) {
        if(!site(game,plan,tree,cell,routes))continue;
        bool door=false;for(Cell cut:doors)if(distance(cell,cut)<=opening)door=true;
        if(door || (roll.value==1 && random_u32(game)%3==0))continue;
        auto& tile=*game.stage.at(cell);saved.push_back({cell,tile});tile=wood_tile(TileMaterial::Root);
    }
    finish(game,plan,roll,saved,"Burnable/cuttable partition; independently rolled openings and required routes retained");
}
}

void compose_tree_hollow(Game& game,FloorPlan& plan,GiantTree& tree,GenerationTrace* trace) {
    const Cell center=tree.canopy.start+Cell{tree.canopy.length/2,tree.canopy.width/2};
    const auto routes=generation_walking_routes(game,plan);
    if(tree.hollow==SplitHeart) {
        const Cell axis=directions[random_u32(game)%4],side{-axis.y,axis.x};
        const int offset=static_cast<int>(random_u32(game)%9)-4;
        root_partition(game,plan,tree,center-scale(axis,13)+scale(side,offset),center+scale(side,offset+3),center+scale(axis,13)-scale(side,offset),1,routes,trace,true);
    }
    const int count=tree.hollow==RootGalleries ? 5+static_cast<int>(random_u32(game)%4) : 2+static_cast<int>(random_u32(game)%3);
    for(int n=0;n<count;++n) {
        if(tree.hollow==RootGalleries && n<3) {
            const Cell axis=directions[(static_cast<unsigned>(n)+random_u32(game)%2)%4],side{-axis.y,axis.x};
            const int bend=static_cast<int>(random_u32(game)%7)-3;
            root_partition(game,plan,tree,center+scale(axis,12)+scale(side,bend),center+scale(axis,4)+scale(side,bend+2),center+scale(side,8)+scale(axis,bend),1,routes,trace,true);
            continue;
        }
        const Cell a=center+Cell{static_cast<int>(random_u32(game)%23)-11,static_cast<int>(random_u32(game)%23)-11};
        const Cell dir=directions[random_u32(game)%4],side{-dir.y,dir.x};
        const int length=tree.hollow==RootGalleries ? 6+static_cast<int>(random_u32(game)%6) : 3+static_cast<int>(random_u32(game)%4);
        root_partition(game,plan,tree,a,a+scale(dir,length/2),a+scale(dir,length)+scale(side,static_cast<int>(random_u32(game)%9)-4),0,routes,trace);
    }
    if(tree.hollow==WetHollow || tree.hollow==RottenHeart) {
        const int pools=2+static_cast<int>(random_u32(game)%3);
        for(int n=0;n<pools;++n) {
            const Cell anchor=center+Cell{static_cast<int>(random_u32(game)%17)-8,static_cast<int>(random_u32(game)%17)-8};
            const WeightedComponent sizes[]{{3,"Small pocket",3},{5,"Broad pocket",5},{7,"Spreading pocket",2}};
            const auto roll=roll_component(game,&plan.report,feature,tree.hollow_component,tree.hollow==WetHollow ? "Seep pool" : "Rotten floor",anchor,sizes);
            const GenerationStep step{trace,game,plan,"Hollow terrain pocket",feature,roll.record};
            const int rx=roll.value,ry=2+static_cast<int>(random_u32(game)%static_cast<unsigned>(rx));
            const std::array polygon{anchor+Cell{-rx,0},anchor+Cell{-rx/2,-ry},anchor+Cell{rx/2,-ry+1},anchor+Cell{rx,2},anchor+Cell{rx/2,ry},anchor+Cell{-rx/2,ry-1}};
            if(roll.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(roll.record)];row.guide.assign(polygon.begin(),polygon.end());row.guide_closed=true;}
            std::vector<Saved> saved;
            for(Cell cell:raster_polygon(polygon,plan.width,plan.height).cells)if(site(game,plan,tree,cell,routes)) {
                auto& tile=*game.stage.at(cell);saved.push_back({cell,tile});tile={tree.hollow==WetHollow ? TileKind::ShallowWater : TileKind::Chasm};
            }
            finish(game,plan,roll,saved,tree.hollow==WetHollow ? "Actual shallow pool; dry route and reward approaches retained" : "Real fall hazard; dry route and reward approaches retained");
        }
    }
    if(tree.hollow_component>=0) {
        std::vector<Cell> interior;
        for(int y=0;y<tree.canopy.width;++y)for(int x=0;x<tree.canopy.length;++x) {
            const Cell cell=tree.canopy.start+Cell{x,y};
            if(tree_ellipse(tree.canopy,cell,3))interior.push_back(cell);
        }
        component_area(&plan.report,{tree.hollow,tree.hollow_component},"Interior composed; branches and terrain pockets have independent rolls",interior);
    }
}
