#include "timber_grove.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "feature_roll.hpp"
#include "four_room_block.hpp"
#include "growth_carving.hpp"
#include "room_frame.hpp"
#include "raster.hpp"
#include "terrain_material.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr auto feature=GenerationFeature::TimberGrove;
Cell at(const TimberGrove& grove,Cell local) {return grove.center+turn_cell(local,grove.turns);}
bool within(const TimberGrove& grove,Cell cell) {
    const Cell d=cell-grove.center;
    return std::abs(d.x)<=20 && std::abs(d.y)<=20 && grove.footprint[static_cast<std::size_t>((d.y+20)*41+d.x+20)]!=0;
}
void connect(Game& game,FloorPlan& plan,TimberGrove& grove,Cell a,Cell b,bool main,GenerationTrace* trace) {
    const WeightedComponent widths[]{{0,"Fuel trail",main ? 0U : 10U},{1,"Narrow mineral break",main ? 5U : 3U},{2,"Broad mineral break",main ? 3U : 1U}};
    const auto roll=roll_component(game,&plan.report,feature,grove.component,main ? "Main firebreak" : "Glade connection",a,widths);
    const GenerationStep step{trace,game,plan,"Grove trail",feature,roll.record};
    const Cell bend{(a.x+b.x)/2+static_cast<int>(random_u32(game)%9)-4,(a.y+b.y)/2+static_cast<int>(random_u32(game)%9)-4};
    if(roll.record>=0)plan.report.components[static_cast<std::size_t>(roll.record)].guide={a,bend,b};
    std::vector<Cell> changed;
    const auto carve=[&](Cell from,Cell to) {
        for(Cell cell:raster_line(from,to,std::max(1,roll.value),plan.width,plan.height).cells) {
            if(!within(grove,cell))continue;
            auto& tile=*game.stage.at(cell);
            // Fuel trails may join mineral breaks, but never paint fuel over one.
            if(tile.kind==TileKind::Ruin && !roll.value)continue;
            tile={roll.value ? TileKind::Ruin : TileKind::Grass};changed.push_back(cell);
        }
    };
    carve(a,bend);carve(bend,b);
    component_result(&plan.report,roll,roll.value ? "Bare mineral trail connects glades" : "Burnable ground connects glades",changed);
}
void glade(Game& game,FloorPlan& plan,TimberGrove& grove,Cell anchor,GenerationTrace* trace) {
    const WeightedComponent forms[]{{0,"Broad opening",grove.shape==0 ? 6U : 2U},{1,"Narrow reach",grove.shape==2 ? 7U : 2U},{2,"Small clearing",grove.shape==1 ? 6U : 2U}};
    const auto roll=roll_component(game,&plan.report,feature,grove.component,"Grove glade",anchor,forms);
    const GenerationStep step{trace,game,plan,"Grove glade",feature,roll.record};
    const int rx=roll.value==0 ? 9+static_cast<int>(random_u32(game)%5) : roll.value==1 ? 11+static_cast<int>(random_u32(game)%4) : 5+static_cast<int>(random_u32(game)%4);
    const int ry=roll.value==0 ? 8+static_cast<int>(random_u32(game)%5) : roll.value==1 ? 5+static_cast<int>(random_u32(game)%3) : 5+static_cast<int>(random_u32(game)%3);
    const int turns=static_cast<int>(random_u32(game)%4),bite=static_cast<int>(random_u32(game)%4);
    std::array polygon{Cell{-rx,0},Cell{-rx/2,-ry},Cell{rx/2,-ry+bite},Cell{rx,1},Cell{rx/2,ry},Cell{-rx/2,ry-bite}};
    for(auto& p:polygon)p=anchor+turn_cell(p,turns);
    if(roll.record>=0){auto& row=plan.report.components[static_cast<std::size_t>(roll.record)];row.guide.assign(polygon.begin(),polygon.end());row.guide_closed=true;}
    TimberGlade clearing;clearing.center=anchor;clearing.component=roll.record;
    for(Cell cell:raster_polygon(polygon,plan.width,plan.height).cells)if(within(grove,cell)) {
        if(!plan.protected_cell(cell))*game.stage.at(cell)={TileKind::Grass};
        clearing.ground.push_back(cell);
    }
    component_result(&plan.report,roll,"Irregular opening cut through timber",clearing.ground);
    grove.glades.push_back(std::move(clearing));
}
void spring(Game& game,FloorPlan& plan,TimberGrove& grove,GenerationTrace* trace) {
    const WeightedComponent choices[]{{0,"Dry grove",2},{4,"Short wall spring",4},{7,"Long wall spring",3}};
    const auto roll=roll_component(game,&plan.report,feature,grove.component,"Grove spring",grove.center,choices);
    const GenerationStep step{trace,game,plan,"Grove spring",feature,roll.record};
    if(!roll.value){component_result(&plan.report,roll,"No added spring");return;}
    struct Source {Cell cell,flow;int direction;};std::vector<Source> sources;
    constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
    for(int y=-18;y<=18;++y)for(int x=-18;x<=18;++x)for(int d=0;d<4;++d) {
        const Cell cell=grove.center+Cell{x,y},flow=directions[d],side{-flow.y,flow.x};
        if(game.stage.at_or_border(cell-flow).kind!=TileKind::Wall)continue;
        bool safe=true;
        for(int i=0;i<roll.value;++i)for(int j=-1;j<=1;++j) {
            const Cell c=cell+Cell{flow.x*i+side.x*j,flow.y*i+side.y*j};
            if(!within(grove,c) || plan.protected_cell(c) || game.stage.at_or_border(c).kind!=TileKind::Grass)safe=false;
        }
        if(safe)sources.push_back({cell,flow,d+1});
    }
    if(sources.empty()){component_result(&plan.report,roll,"No wall-backed pool fits off the bare routes");return;}
    const auto source=sources[random_u32(game)%sources.size()];const Cell side{-source.flow.y,source.flow.x};std::vector<Cell> placed;
    for(int i=0;i<roll.value;++i)for(int j=-1;j<=1;++j) {
        if((i==0 || i==roll.value-1) && j)continue;
        const Cell cell=source.cell+Cell{source.flow.x*i+side.x*j,source.flow.y*i+side.y*j};
        auto& tile=*game.stage.at(cell);tile={cell==source.cell ? TileKind::Spring : TileKind::ShallowWater};tile.current=static_cast<std::uint8_t>(source.direction);placed.push_back(cell);
    }
    grove.spring=source.cell;component_result(&plan.report,roll,"Wall-backed source feeds a real shallow refuge",placed);
}
}

void plan_timber_grove(Game& game,FloorPlan& plan) {
    if(!roll_generation_feature(game,plan,feature))return;
    TimberGrove grove;
    const auto center=reserve_four_rooms(game,plan,grove.rooms,&plan.report.features.back().candidate_count);
    if(!center){feature_failed(plan,"No eligible four-room block; objectives and earlier habitats are excluded");return;}
    grove.center=*center;grove.turns=static_cast<int>(random_u32(game)%4);
    const WeightedComponent forms[]{{0,"Overlapping woodland",4},{1,"Linked glades",4},{2,"Wooded ridges",3}};
    const auto shape=roll_component(game,&plan.report,feature,-1,"Grove structure",grove.center,forms);
    grove.shape=shape.value;grove.component=shape.record;plan.timber_groves.push_back(grove);
    feature_reserved(plan,grove.rooms,forms[grove.shape].name);
}

void carve_timber_grove(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    for(auto& grove:plan.timber_groves) {
        // Preserve existing route sockets. Everything between them is recomposed.
        const auto saved=game.stage.tiles;
        const auto edge=[&](){return 17+static_cast<int>(random_u32(game)%4);};
        std::array outline{Cell{-edge(),-7},Cell{-edge(),-edge()},Cell{-5,-edge()},Cell{7,-edge()},Cell{edge(),-edge()},Cell{edge(),-5},
            Cell{edge(),7},Cell{edge(),edge()},Cell{5,edge()},Cell{-7,edge()},Cell{-edge(),edge()},Cell{-edge(),5}};
        // Recess alternating corners so timber doesn't end at a rectangular stamp.
        for(int i:{1,4,7,10}) {auto& cell=outline[static_cast<std::size_t>(i)];cell.x=cell.x*3/4;cell.y=cell.y*3/4;}
        for(auto& cell:outline)cell=at(grove,cell);
        if(grove.component>=0){auto& row=plan.report.components[static_cast<std::size_t>(grove.component)];row.guide.assign(outline.begin(),outline.end());row.guide_closed=true;}
        for(Cell cell:raster_polygon(outline,plan.width,plan.height).cells) {
            const Cell local=cell-grove.center;
            if(std::abs(local.x)<=20 && std::abs(local.y)<=20)grove.footprint[static_cast<std::size_t>((local.y+20)*41+local.x+20)]=1;
        }
        for(int y=-20;y<=20;++y)for(int x=-20;x<=20;++x) {
            const Cell cell=grove.center+Cell{x,y};
            if(within(grove,cell) && !plan.protected_cell(cell))*game.stage.at(cell)=wood_tile(TileMaterial::Tree);
        }
        grove.entry=at(grove,{-16,static_cast<int>(random_u32(game)%13)-6});
        grove.cache=at(grove,{15,static_cast<int>(random_u32(game)%13)-6});
        const int count=(grove.shape==1 ? 8 : 5)+static_cast<int>(random_u32(game)%4);
        std::vector<Cell> anchors;
        for(int attempt=0;attempt<100 && static_cast<int>(anchors.size())<count;++attempt) {
            const Cell cell=grove.center+Cell{static_cast<int>(random_u32(game)%31)-15,static_cast<int>(random_u32(game)%31)-15};
            if(!within(grove,cell))continue;
            if(std::any_of(anchors.begin(),anchors.end(),[&](Cell old){return distance(cell,old)<7;}))continue;
            anchors.push_back(cell);glade(game,plan,grove,cell,trace);
        }
        connect(game,plan,grove,grove.entry,grove.cache,true,trace);
        // Each opening joins the nearest earlier opening or the main trail.
        std::vector<Cell> joined{grove.entry,grove.cache};
        for(Cell cell:anchors) {
            const Cell other=*std::min_element(joined.begin(),joined.end(),[&](Cell a,Cell b){return distance(cell,a)<distance(cell,b);});
            connect(game,plan,grove,cell,other,false,trace);joined.push_back(cell);
        }
        for(int y=-20;y<=20;++y)for(int x=-20;x<=20;++x) {
            const Cell cell=grove.center+Cell{x,y};if(within(grove,cell) && plan.protected_cell(cell))*game.stage.at(cell)={TileKind::Ruin};
        }
        if(!generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
            game.stage.tiles=saved;grove.glades.clear();grove.entry=grove.cache=grove.center;
            component_result(&plan.report,{grove.shape,grove.component},"Geometry rolled back: required route or exit lock changed");
            for(auto& decision:plan.report.features)if(decision.feature==feature) {
                decision.outcome=GenerationOutcome::Failed;decision.reason="Grove carving rolled back; original room geometry retained";
            }
            for(int room:grove.rooms)plan.rooms[static_cast<std::size_t>(room)].landmark=false;
            continue;
        } else component_result(&plan.report,{grove.shape,grove.component},"Connected irregular glades; child trails and fuel vary independently",anchors);
        spring(game,plan,grove,trace);
        for(int y=-20;y<=20;++y)for(int x=-20;x<=20;++x) {
            const Cell cell=grove.center+Cell{x,y};const auto& tile=game.stage.at_or_border(cell);
            if(within(grove,cell) && walkable(tile))grove.ground.push_back(cell);
            if(tile.kind==TileKind::Ruin || tile.kind==TileKind::Spring || tile.kind==TileKind::ShallowWater)grove.firebreaks.push_back(cell);
        }
        dress_timber_grove(game,plan,grove,trace);
        for(int y=-21;y<=21;++y)for(int x=-21;x<=21;++x) {
            const Cell cell=grove.center+Cell{x,y};plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
        }
    }
}
