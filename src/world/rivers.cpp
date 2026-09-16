#include "rivers.hpp"
#include "generation_trace.hpp"
#include "feature_roll.hpp"
#include "components.hpp"
#include "routed_path.hpp"
#include "raster.hpp"
#include "growth_carving.hpp"
#include "water.hpp"
#include "../scenery/roof.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
bool eligible(const Game& game,const FloorPlan& plan,Cell cell) {
    const auto* t=game.stage.at(cell);
    if (!t || cell.x<1 || cell.y<1 || cell.x>=plan.width-1 || cell.y>=plan.height-1 ||
        t->contents!=ItemKind::None || t->prop.kind!=PropKind::None || t->material==TileMaterial::Root) return false;
    if (t->kind==TileKind::Wall && (t->break_rule==BreakRule::Unbreakable || supports_wall_spring(game.stage,cell))) return false;
    if (t->kind!=TileKind::Wall && t->kind!=TileKind::Grass && t->kind!=TileKind::Empty && t->kind!=TileKind::Ruin &&
        t->kind!=TileKind::ShallowWater && t->kind!=TileKind::Spring) return false;
    for (int index:{0,plan.exit_room,plan.objective_room,plan.secret_room}) {
        if (index<0) continue;
        const auto& room=plan.rooms[static_cast<std::size_t>(index)];
        if (std::abs(cell.x-room.center.x)<=room.half_width+2 && std::abs(cell.y-room.center.y)<=room.half_height+2) return false;
    }
    if (distance(cell,plan.door)<4) return false;
    for (const auto& roof:game.stage.roofs) if (roof_covers(roof,cell)) return false;
    return true;
}
std::uint8_t flow_code(Cell d) {return static_cast<std::uint8_t>(d.x>0 ? 1 : d.y>0 ? 2 : d.x<0 ? 3 : 4);}
struct Outlet {Cell cell,flow;};
struct Source {Cell cell,flow;};
}
void carve_forest_river(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    if (!roll_generation_feature(game,plan,GenerationFeature::River)) return;
    const auto count=game.stage.tiles.size();
    std::vector<std::uint8_t> allowed(count);std::vector<Source> springs,sources;
    std::vector<Outlet> drains,drops;
    for (int y=1;y<plan.height-1;++y) for (int x=1;x<plan.width-1;++x) {
        const Cell c{x,y};if (!eligible(game,plan,c)) continue;
        allowed[static_cast<std::size_t>(y*plan.width+x)]=1;
        const auto& tile=*game.stage.at(c);
        if (tile.kind==TileKind::Spring || ((tile.kind==TileKind::Grass || tile.kind==TileKind::Empty) && x>4 && y>4 && x<plan.width-5 && y<plan.height-5))
            for (Cell d:sides) if (game.stage.at_or_border(c-d).kind==TileKind::Wall && eligible(game,plan,c+d)) {
                (tile.kind==TileKind::Spring ? springs : sources).push_back({c,d});
            }
        for (Cell d:sides) {
            const Cell next=c+d;const auto& t=game.stage.at_or_border(next);
            if (t.kind==TileKind::Chasm) drops.push_back({c,d});
            if (next.x==0 || next.y==0 || next.x==plan.width-1 || next.y==plan.height-1) drains.push_back({c,d});
        }
    }
    if (!springs.empty()) sources=springs;
    plan.report.features.back().candidate_count=static_cast<int>(sources.size());
    const WeightedComponent styles[]{{0,"Direct brook",3},{1,"Meandering stream",5},
        {2,"Broad shallows",game.run.floor==1 ? 1U : 3U},{3,"Circulating river",game.run.floor==1 ? 0U : 3U}};
    const auto style=roll_component(game,&plan.report,GenerationFeature::River,-1,"River shape",
        sources.empty() ? plan.rooms.front().center : sources.front().cell,styles);
    if (style.value==3) {
        // A circulating channel must not consume an existing wall-backed source.
        for (std::size_t i=0;i<count;++i) if (game.stage.tiles[i].kind==TileKind::Spring) allowed[i]=0;
        carve_forest_river_loop(game,plan,trace,allowed,style);return;
    }
    if (sources.empty() || (drains.empty() && drops.empty())) {
        component_result(&plan.report,style,"No eligible source/outlet pair");
        auto& decision=plan.report.features.back();decision.outcome=GenerationOutcome::Failed;decision.reason="No eligible source/outlet pair";return;
    }
    const WeightedComponent ends[]{{0,"Boundary drain",drains.empty() ? 0U : 3U},{1,"Chasm spill",drops.empty() ? 0U : 2U}};
    for (int attempt=0;attempt<6;++attempt) {
        const auto spring=sources[random_u32(game)%sources.size()];
        const Cell source=spring.cell,front=source+spring.flow,back=source-spring.flow;
        const auto outlet_roll=roll_component(game,&plan.report,GenerationFeature::River,style.record,"Outlet attempt",source,ends);
        const GenerationStep step{trace,game,plan,"River attempt",GenerationFeature::River,outlet_roll.record};
        const auto& pool=outlet_roll.value==0 ? drains : drops;
        std::vector<Outlet> far;
        for (const auto& outlet:pool) if (distance(source,outlet.cell)>=30) far.push_back(outlet);
        if (far.empty()) {component_result(&plan.report,outlet_roll,"No outlet at least 30 tiles from source");continue;}
        const auto outlet=far[random_u32(game)%far.size()];
        if (outlet_roll.record>=0) plan.report.components[static_cast<std::size_t>(outlet_roll.record)].guide={source,outlet.cell};
        // Steer toward displaced waypoints. The cost field can bend around real
        // structures; the guide is a preference rather than a pasted channel.
        std::vector<Cell> guide{source};
        const Cell delta=outlet.cell-source;const bool horizontal=std::abs(delta.x)>std::abs(delta.y);
        const int divisions=style.value==0 ? 2 : 5;
        for (int i=1;i<divisions;++i) {
            Cell p{source.x+delta.x*i/divisions,source.y+delta.y*i/divisions};
            const int drift=style.value==0 ? 0 : static_cast<int>(random_u32(game)%29)-14;
            if (horizontal) p.y+=drift;else p.x+=drift;
            p.x=std::clamp(p.x,1,plan.width-2);p.y=std::clamp(p.y,1,plan.height-2);guide.push_back(p);
        }
        guide.push_back(outlet.cell);
        if (style.record>=0) plan.report.components[static_cast<std::size_t>(style.record)].guide=guide;
        std::vector<Cell> guide_cells;
        for (std::size_t i=1;i<guide.size();++i) {
            const auto line=raster_line(guide[i-1],guide[i],0,plan.width,plan.height);
            guide_cells.insert(guide_cells.end(),line.cells.begin(),line.cells.end());
        }
        std::vector<unsigned> costs(count,0);
        for (int y=1;y<plan.height-1;++y) for (int x=1;x<plan.width-1;++x) {
            const auto index=static_cast<std::size_t>(y*plan.width+x);if (!allowed[index]) continue;
            int separation=1000;for (Cell p:guide_cells) separation=std::min(separation,distance({x,y},p));
            costs[index]=10U+static_cast<unsigned>(std::min(40,separation)*4)+
                (game.stage.at({x,y})->kind==TileKind::Wall ? 3U : 0U);
        }
        costs[static_cast<std::size_t>(source.y*plan.width+source.x)]=0;
        costs[static_cast<std::size_t>(back.y*plan.width+back.x)]=0;
        auto path=route_cost_field(front,outlet.cell,plan.width,plan.height,costs);
        if (!path.empty()) path.insert(path.begin(),source);
        if (path.size()<31) {component_result(&plan.report,outlet_roll,"No sufficiently long connected route to outlet");continue;}
        if (outlet_roll.record>=0) plan.report.components[static_cast<std::size_t>(outlet_roll.record)].guide=path;
        std::vector<int> owner(count,-1),bank_owner(count,-1);
        int width=1,remaining=0;
        for (std::size_t i=0;i<path.size();++i) {
            if (remaining--<=0) {width=style.value==0 ? 1 : 1+static_cast<int>(random_u32(game)%(style.value==2 ? 3U : 2U));remaining=4+static_cast<int>(random_u32(game)%9);}
            const int radius=(i==0 || i+1==path.size()) ? 0 : width;
            for (Cell c:raster_line(path[i],path[i],radius+2,plan.width,plan.height).cells) {
                const auto index=static_cast<std::size_t>(c.y*plan.width+c.x);if (allowed[index] && c!=back) bank_owner[index]=static_cast<int>(i);
            }
            for (Cell c:raster_line(path[i],path[i],radius,plan.width,plan.height).cells) {
                const auto index=static_cast<std::size_t>(c.y*plan.width+c.x);
                if (allowed[index] && c!=back && (owner[index]<0 || distance(c,path[i])<distance(c,path[static_cast<std::size_t>(owner[index])]))) owner[index]=static_cast<int>(i);
            }
        }
        struct Saved {Cell cell;Tile tile;};std::vector<Saved> saved;
        RiverPlan river;river.path=path;river.source=source;river.outlet=outlet.cell;river.outflow=outlet.flow;river.component=outlet_roll.record;
        for (int y=1;y<plan.height-1;++y) for (int x=1;x<plan.width-1;++x) {
            const Cell c{x,y};const auto index=static_cast<std::size_t>(y*plan.width+x);auto* tile=game.stage.at(c);
            if (bank_owner[index]<0) continue;
            saved.push_back({c,*tile});
            if (owner[index]>=0) {
                *tile={c==source ? TileKind::Spring : TileKind::ShallowWater};river.channel.push_back(c);
            } else {
                if (tile->kind==TileKind::Wall) *tile={TileKind::Grass};
                if (tile->kind==TileKind::Grass || tile->kind==TileKind::Empty || tile->kind==TileKind::Ruin) river.banks.push_back(c);
            }
        }
        // Route all channel cells toward the outlet through a single BFS tree.
        // Unlike nearest-segment arrows this cannot create circulation at bends.
        std::vector<bool> seen(count,false);std::vector<Cell> queue{outlet.cell};seen[static_cast<std::size_t>(outlet.cell.y*plan.width+outlet.cell.x)]=true;
        for (std::size_t i=0;i<queue.size();++i) for (Cell d:sides) {
            if (queue[i]==source) continue;
            const Cell c=queue[i]+d;if (!game.stage.in_bounds(c)) continue;
            const auto index=static_cast<std::size_t>(c.y*plan.width+c.x);
            if (seen[index] || owner[index]<0) continue;
            seen[index]=true;game.stage.at(c)->current=flow_code(queue[i]-c);queue.push_back(c);
        }
        game.stage.at(source)->current=flow_code(spring.flow);
        game.stage.at(outlet.cell)->current=flow_code(outlet.flow);
        if (queue.size()!=river.channel.size() || !generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
            for (const auto& old:saved) *game.stage.at(old.cell)=old.tile;
            component_result(&plan.report,outlet_roll,"Rolled back: disconnected water or required route/lock changed");continue;
        }
        component_result(&plan.report,outlet_roll,"Connected shallow channel and standable banks",river.channel);
        component_result(&plan.report,style,"River built",river.banks);
        auto& decision=plan.report.features.back();decision.outcome=GenerationOutcome::Built;decision.reason="Shallow crossings remain walkable; bank/channel carving retained exit lock";
        decision.variant=std::string(styles[style.value].name)+" / "+ends[outlet_roll.value].name;
        Cell low{plan.width,plan.height},high{};
        for (Cell c:river.channel) {low.x=std::min(low.x,c.x);low.y=std::min(low.y,c.y);high.x=std::max(high.x,c.x+1);high.y=std::max(high.y,c.y+1);}
        decision.regions.push_back({low,high});
        for (Cell c:river.channel) plan.protected_cells[static_cast<std::size_t>(c.y*plan.width+c.x)]=1;
        plan.rivers.push_back(std::move(river));return;
    }
    component_result(&plan.report,style,"All outlet attempts failed");
    auto& decision=plan.report.features.back();decision.outcome=GenerationOutcome::Failed;decision.reason="Six source/outlet attempts exhausted; see child reasons";
}
