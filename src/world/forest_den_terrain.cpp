#include "forest_den.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "raster.hpp"
#include "currents.hpp"
#include "terrain_material.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr auto feature=GenerationFeature::BearDen;
Cell scale(Cell c,int n){return {c.x*n,c.y*n};}
Cell at(const FloorPlan& plan,const ForestDen& den,int room,Cell offset) {
    return plan.rooms[static_cast<std::size_t>(room)].center+scale(den.along,offset.x)+scale(den.across,offset.y);
}
bool footprint(const FloorPlan& plan,const ForestDen& den,Cell cell) {
    for(int i:{den.a,den.b}) {
        const auto& r=plan.rooms[static_cast<std::size_t>(i)];
        if(std::abs(cell.x-r.center.x)<=r.half_width && std::abs(cell.y-r.center.y)<=r.half_height)return true;
    }
    const Cell a=plan.rooms[static_cast<std::size_t>(den.a)].center,b=plan.rooms[static_cast<std::size_t>(den.b)].center;
    const Cell d=cell-a;const int x=d.x*den.along.x+d.y*den.along.y;
    const int length=std::abs(b.x-a.x)+std::abs(b.y-a.y);
    return x>=0 && x<=length && std::abs(d.x*den.across.x+d.y*den.across.y)<=6;
}
void floor(Game& game,const FloorPlan& plan,const ForestDen& den,Cell cell) {
    if(auto* tile=game.stage.at(cell);tile && footprint(plan,den,cell) && !plan.protected_cell(cell))*tile={TileKind::Grass};
}
std::vector<Cell> path(Game& game,const FloorPlan& plan,const ForestDen& den,Cell a,Cell b,int width) {
    std::vector<Cell> cells;
    for(Cell cell:raster_line(a,b,width,plan.width,plan.height).cells)if(footprint(plan,den,cell)) {
        floor(game,plan,den,cell);cells.push_back(cell);
    }
    return cells;
}
std::vector<Cell> polygon(Game& game,const ForestDen& den,Cell anchor,int rx,int ry) {
    const int left=1+static_cast<int>(random_u32(game)%3),right=1+static_cast<int>(random_u32(game)%3);
    const std::array offsets{Cell{-rx,-ry/2},Cell{-rx+left,-ry},Cell{rx-right,-ry+1},Cell{rx,0},Cell{rx-right,ry},Cell{-rx+left,ry-1}};
    std::vector<Cell> outline;for(Cell c:offsets)outline.push_back(anchor+scale(den.along,c.x)+scale(den.across,c.y));
    return outline;
}
void pocket(Game& game,FloorPlan& plan,const ForestDen& den,Cell anchor,Cell link,int rx,int ry,
    int parent,const char* slot,GenerationTrace* trace) {
    const WeightedComponent shapes[]{{0,"Broad bowl",3},{1,"Long shelter",3},{2,"Uneven alcove",4}};
    const auto shape=roll_component(game,&plan.report,feature,parent,slot,anchor,shapes);
    const GenerationStep checkpoint{trace,game,plan,"Den terrain pocket",feature,shape.record};
    if(shape.value==1){rx=std::min(9,rx+1);ry=std::max(3,ry-1);}
    else if(shape.value==2)rx=std::max(3,rx-1);
    const auto outline=polygon(game,den,anchor,rx,ry);
    std::vector<Cell> cells;
    for(Cell c:raster_polygon(outline,plan.width,plan.height).cells)if(footprint(plan,den,c)) {
        floor(game,plan,den,c);cells.push_back(c);
    }
    path(game,plan,den,link,anchor,2);
    component_area(&plan.report,shape,"Connected polygon pocket; required corridor sockets retained",cells);
    if(shape.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(shape.record)];row.guide=outline;row.guide_closed=true;}
}

std::vector<bool> reachable(const Stage& stage,Cell start) {
    std::vector<bool> seen(stage.tiles.size());std::vector<Cell> queue{start};
    for(std::size_t i=0;i<queue.size();++i) {
        const Cell c=queue[i];if(!walkable(stage.at_or_border(c)))continue;
        const auto index=static_cast<std::size_t>(c.y*stage.width+c.x);
        if(seen[index])continue;
        seen[index]=true;
        for(Cell d:{Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}})queue.push_back(c+d);
    }
    return seen;
}
void ribs(Game& game,FloorPlan& plan,const ForestDen& den,int layout,GenerationTrace* trace) {
    const Cell home=plan.rooms[static_cast<std::size_t>(den.b)].center;
    const int count=layout==0 ? 1 : layout==1 ? 3 : 2;
    for(int n=0;n<count;++n) {
        const Cell origin=at(plan,den,den.b,{-5+static_cast<int>(random_u32(game)%11),-5+static_cast<int>(random_u32(game)%5)});
        const WeightedComponent styles[]{{0,"Open floor",layout==0 ? 5U : 1U},{1,"Broken root rib",3},{2,"Root screen with a gap",layout==1 ? 6U : 3U}};
        const auto style=roll_component(game,&plan.report,feature,den.component,"Hollow root rib",origin,styles);
        const GenerationStep checkpoint{trace,game,plan,"Den interior root rib",feature,style.record};
        if(!style.value){component_area(&plan.report,style,"Open floor retained");continue;}
        const Cell axis=random_u32(game)%2 ? den.along : den.across;
        const int length=3+static_cast<int>(random_u32(game)%4);
        const Cell a=origin-scale(axis,length),b=origin+scale(axis,length);
        const auto shape=raster_line(a,b,0,plan.width,plan.height);
        const Cell opening=origin+scale(axis,static_cast<int>(random_u32(game)%5)-2);
        if(style.record>=0)plan.report.components[static_cast<std::size_t>(style.record)].guide={a,b};
        const auto before=reachable(game.stage,home);
        std::vector<std::pair<Cell,Tile>> saved;
        for(Cell c:shape.cells) {
            auto* tile=game.stage.at(c);
            if(!tile || !footprint(plan,den,c) || plan.protected_cell(c) || distance(c,opening)<=1 ||
                tile->kind!=TileKind::Grass || (style.value==1 && random_u32(game)%3==0))continue;
            saved.push_back({c,*tile});*tile=wood_tile(TileMaterial::Root);
        }
        const auto after=reachable(game.stage,home);bool cut=false;
        for(std::size_t i=0;i<before.size();++i)if(before[i] && walkable(game.stage.tiles[i]) && !after[i]){cut=true;break;}
        std::vector<Cell> cells;
        for(const auto& [cell,tile]:saved) {if(cut)*game.stage.at(cell)=tile;else cells.push_back(cell);}
        component_area(&plan.report,style,cut ? "Rolled back: root rib isolated existing ground" :
            cells.empty() ? "No unreserved floor along this rib" : "Burnable cover; gap and surviving floor remain connected",cells);
    }
}

void stream(Game& game,FloorPlan& plan,ForestDen& den,GenerationTrace* trace) {
    const Cell a=plan.rooms[static_cast<std::size_t>(den.a)].center,b=plan.rooms[static_cast<std::size_t>(den.b)].center;
    const WeightedComponent modes[]{{0,"Spring pool",3},{1,"Meandering side creek",5},{2,"Seeping banks",2}};
    const auto mode=roll_component(game,&plan.report,feature,den.component,"Den water reach",a,modes);
    const GenerationStep checkpoint{trace,game,plan,"Den stream and dry crossings",feature,mode.record};
    const int length=std::abs(b.x-a.x)+std::abs(b.y-a.y);
    const int offset=3+static_cast<int>(random_u32(game)%3);
    std::vector<Cell> guide{at(plan,den,den.a,{-6,offset})},proposed;
    if(mode.value==1) {
        for(int x=-2;x<=length+3;x+=4) {
            const int y=std::clamp(offset+static_cast<int>(random_u32(game)%3)-1,2,6);
            guide.push_back(at(plan,den,den.a,{x,y}));
        }
        for(std::size_t i=1;i<guide.size();++i) {
            const auto segment=raster_line(guide[i-1],guide[i],random_u32(game)%3==0 ? 1 : 0,plan.width,plan.height);
            proposed.insert(proposed.end(),segment.cells.begin(),segment.cells.end());
        }
    } else {
        const int pools=mode.value==0 ? 1 : 2+static_cast<int>(random_u32(game)%3);
        for(int i=0;i<pools;++i) {
            const Cell center=at(plan,den,i>0 && random_u32(game)%2 ? den.b : den.a,
                {-3+static_cast<int>(random_u32(game)%8),offset});
            const auto outline=polygon(game,den,center,mode.value==0 ? 4 : 2+static_cast<int>(random_u32(game)%2),2);
            const auto pool=raster_polygon(outline,plan.width,plan.height);
            proposed.insert(proposed.end(),pool.cells.begin(),pool.cells.end());
            guide.push_back(center);
        }
    }
    std::vector<Cell> cells,rejected;
    for(Cell c:proposed) {
        auto* tile=game.stage.at(c);
        if(!tile || !footprint(plan,den,c))continue;
        if(plan.protected_cell(c)){rejected.push_back(c);continue;}
        if(std::find(cells.begin(),cells.end(),c)!=cells.end())continue;
        // Pools also carve their banks, so a source cannot be hidden in solid wood.
        for(Cell d:{Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}})floor(game,plan,den,c+d);
        *tile={TileKind::ShallowWater};tile->current=make_current(den.along);cells.push_back(c);
    }
    // Later bank dilation may have touched an earlier wet cell.
    for(Cell c:cells){auto& tile=*game.stage.at(c);tile={TileKind::ShallowWater};tile.current=make_current(den.along);}
    den.spring=cells.empty() ? a : cells.front();
    if(!cells.empty())game.stage.at(den.spring)->kind=TileKind::Spring;
    component_area(&plan.report,mode,cells.empty() ? "No unreserved water site" : "Shallow water and source; required dry crossings retained",cells);
    if(mode.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(mode.record)];row.guide=guide;row.rejected_cells=rejected;}
}

void beds(Game& game,FloorPlan& plan,ForestDen& den,GenerationTrace* trace) {
    const Cell center=plan.rooms[static_cast<std::size_t>(den.b)].center;
    const unsigned later=biome_stage(game.run.floor)>=2 ? 1U : 0U;
    const WeightedComponent counts[]{{3,"Three bed sites",3},{4,"Four bed sites",4},{6,"Six bed sites",4*later},{8,"Eight bed sites",2*later}};
    const auto count=roll_component(game,&plan.report,feature,den.component,"Dry sleeping sites",center,counts);
    const GenerationStep checkpoint{trace,game,plan,"Den sleeping sites",feature,count.record};
    std::vector<Cell> candidates;
    for(int y=-8;y<=0;++y)for(int x=-8;x<=8;++x) {
        const Cell c=at(plan,den,den.b,{x,y});
        if(game.stage.at_or_border(c).kind==TileKind::Grass && !plan.protected_cell(c))candidates.push_back(c);
    }
    for(std::size_t n=candidates.size();n>1;--n)std::swap(candidates[n-1],candidates[random_u32(game)%n]);
    for(Cell c:candidates) {
        if(static_cast<int>(den.beds.size())>=count.value)break;
        if(std::none_of(den.beds.begin(),den.beds.end(),[&](Cell old){return distance(c,old)<3;}))den.beds.push_back(c);
    }
    component_result(&plan.report,count,den.beds.size()==static_cast<std::size_t>(count.value) ? "Beds fitted to dry sheltered ground" : "Reduced by dry floor and bed spacing",den.beds);
    std::erase_if(candidates,[&](Cell c){return std::any_of(den.beds.begin(),den.beds.end(),[&](Cell old){return distance(c,old)<2;});});
    // Reward anchor follows surviving ground, with no fixed side or guaranteed food beside it.
    den.cache=candidates.empty() ? center : candidates.front();
}
}

void carve_forest_den(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    for(auto& den:plan.forest_dens) {
        const Cell a=plan.rooms[static_cast<std::size_t>(den.a)].center,b=plan.rooms[static_cast<std::size_t>(den.b)].center;
        const WeightedComponent layouts[]{{0,"Open banks",4},{1,"Root-separated bays",4},{2,"Branching shelters",3}};
        const auto layout=roll_component(game,&plan.report,feature,-1,"Den terrain composition",a,layouts);
        den.component=layout.record;
        // Replace only this habitat's old room stamps. Protected graph paths and
        // neighboring footprints remain intact; no compulsory central cross.
        for(int index:{den.a,den.b}) {
            const auto& room=plan.rooms[static_cast<std::size_t>(index)];
            for(int y=-9;y<=9;++y)for(int x=-9;x<=9;++x) {
                const Cell c=room.center+Cell{x,y};
                if(auto* tile=game.stage.at(c);tile && !plan.protected_cell(c))*tile={TileKind::Wall,100,0};
            }
        }
        pocket(game,plan,den,a,a,8+static_cast<int>(random_u32(game)%2),8,layout.record,"Approach bowl",trace);
        pocket(game,plan,den,b,b,layout.value==0 ? 8 : 5,layout.value==0 ? 8 : 5,layout.record,"Sleeping hollow",trace);
        const int extras=layout.value==0 ? 1 : layout.value==1 ? 3 : 5;
        for(int i=0;i<extras;++i) {
            const Cell center=at(plan,den,den.b,{-6+static_cast<int>(random_u32(game)%13),-3-static_cast<int>(random_u32(game)%4)});
            pocket(game,plan,den,center,b,3+static_cast<int>(random_u32(game)%2),3,layout.record,"Shelter branch",trace);
        }
        const WeightedComponent widths[]{{3,"Narrow bank",3},{4,"Broad bank",4},{5,"Open bank",3}};
        const auto bank=roll_component(game,&plan.report,feature,layout.record,"Connecting bank",a,widths);
        const Cell bend={(a.x+b.x)/2,(a.y+b.y)/2};
        const Cell shifted=bend+scale(den.across,static_cast<int>(random_u32(game)%5)-2);
        auto bank_cells=path(game,plan,den,a,shifted,bank.value);
        for(Cell c:path(game,plan,den,shifted,b,bank.value))
            if(std::find(bank_cells.begin(),bank_cells.end(),c)==bank_cells.end())bank_cells.push_back(c);
        component_area(&plan.report,bank,"Bent dry bank joins the two footprints",bank_cells);
        if(bank.record>=0)plan.report.components[static_cast<std::size_t>(bank.record)].guide={a,shifted,b};
        if(trace)trace->capture_detail("Den connecting bank",feature,bank.record,game,plan);
        ribs(game,plan,den,layout.value,trace);
        stream(game,plan,den,trace);beds(game,plan,den,trace);
        std::vector<Cell> ground;
        for(int y=std::min(a.y,b.y)-9;y<=std::max(a.y,b.y)+9;++y)
            for(int x=std::min(a.x,b.x)-9;x<=std::max(a.x,b.x)+9;++x) {
                const Cell c{x,y};if(!game.stage.in_bounds(c) || !footprint(plan,den,c))continue;
                plan.protected_cells[static_cast<std::size_t>(y*plan.width+x)]=1;
                if(walkable(*game.stage.at(c)))ground.push_back(c);
            }
        // Structural roots follow the carved boundary, not the rectangular room
        // reservation. Unexposed rock blends back into the surrounding forest.
        for(Cell c:ground)for(int y=-2;y<=2;++y)for(int x=-2;x<=2;++x) {
            const Cell edge=c+Cell{x,y};auto* tile=game.stage.at(edge);
            if(tile && footprint(plan,den,edge) && tile->kind==TileKind::Wall && x*x+y*y<=4)
                *tile=wood_tile(TileMaterial::Root);
        }
        component_area(&plan.report,layout,"Connected terrain pockets, rolled banks and sleeping sites",ground);
        if(trace)trace->capture_detail("Den root margins",feature,layout.record,game,plan);
    }
}
