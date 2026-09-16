#include "rivers.hpp"
#include "components.hpp"
#include "generation_trace.hpp"
#include "routed_path.hpp"
#include "raster.hpp"
#include "growth_carving.hpp"
#include <algorithm>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
std::uint8_t flow_code(Cell d) {return static_cast<std::uint8_t>(d.x>0 ? 1 : d.y>0 ? 2 : d.x<0 ? 3 : 4);}
}

bool carve_forest_river_loop(Game& game,FloorPlan& plan,GenerationTrace* trace,
    std::span<const std::uint8_t> allowed,ComponentRoll style) {
    std::vector<Cell> centers;
    const auto index=[&](Cell cell){return static_cast<std::size_t>(cell.y*plan.width+cell.x);};
    for (const auto& room:plan.rooms) if (allowed[index(room.center)]) centers.push_back(room.center);
    for (std::size_t i=centers.size();i>1;--i) std::swap(centers[i-1],centers[random_u32(game)%i]);
    plan.report.features.back().candidate_count=static_cast<int>(centers.size());
    const WeightedComponent sizes[]{{0,"Close eddy",3},{1,"Wide island loop",2},{2,"Long looping channel",2}};
    for (std::size_t attempt=0;attempt<std::min<std::size_t>(centers.size(),8);++attempt) {
        const Cell center=centers[attempt];
        const auto form=roll_component(game,&plan.report,GenerationFeature::River,style.record,"Circulating route",center,sizes);
        const GenerationStep step{trace,game,plan,"Circulating river attempt",GenerationFeature::River,form.record};
        const int rx=5+form.value*3+static_cast<int>(random_u32(game)%4),ry=5+(form.value==1 ? 4 : 0)+static_cast<int>(random_u32(game)%4);
        const int turn=static_cast<int>(random_u32(game)%4);
        std::vector<Cell> guide;
        for (Cell offset:std::array{Cell{rx,0},Cell{rx*2/3,ry*2/3},Cell{0,ry},Cell{-rx*2/3,ry*2/3},
                Cell{-rx,0},Cell{-rx*2/3,-ry*2/3},Cell{0,-ry},Cell{rx*2/3,-ry*2/3}}) {
            offset.x+=static_cast<int>(random_u32(game)%3)-1;
            offset.y+=static_cast<int>(random_u32(game)%3)-1;
            for (int i=0;i<turn;++i) offset={-offset.y,offset.x};
            guide.push_back(center+offset);
        }
        if (random_u32(game)%2) std::reverse(guide.begin(),guide.end());
        if (form.record>=0) {auto& child=plan.report.components[static_cast<std::size_t>(form.record)];child.guide=guide;child.guide_closed=true;}
        std::vector<Cell> guide_cells;
        for (std::size_t i=0;i<guide.size();++i) {
            const auto line=raster_line(guide[i],guide[(i+1)%guide.size()],0,plan.width,plan.height);
            guide_cells.insert(guide_cells.end(),line.cells.begin(),line.cells.end());
        }
        std::vector<unsigned> costs(allowed.size());
        for (int y=1;y<plan.height-1;++y) for (int x=1;x<plan.width-1;++x) {
            const Cell c{x,y};if (!allowed[index(c)]) continue;
            int separation=1000;for (Cell p:guide_cells) separation=std::min(separation,distance(c,p));
            if (separation<=5) costs[index(c)]=10U+static_cast<unsigned>(separation*8);
        }
        auto path=route_closed_cost_field(guide,plan.width,plan.height,costs);
        if (path.size()<24 || path.size()>200) {component_result(&plan.report,form,"No closed route through eligible terrain");continue;}
        std::vector<std::uint8_t> channel(allowed.size()),banks(allowed.size());
        for (std::size_t i=0;i<path.size();++i) {
            const int radius=(i/5)%3==1 ? 1 : 0;
            for (Cell c:raster_line(path[i],path[i],radius+2,plan.width,plan.height).cells) if (allowed[index(c)]) banks[index(c)]=1;
            for (Cell c:raster_line(path[i],path[i],radius,plan.width,plan.height).cells) if (allowed[index(c)]) channel[index(c)]=1;
        }
        struct Saved {Cell cell;Tile tile;};std::vector<Saved> saved;
        RiverPlan river;river.path=path;river.source=river.outlet=path.front();river.component=form.record;river.loop=true;
        for (int y=1;y<plan.height-1;++y) for (int x=1;x<plan.width-1;++x) {
            const Cell c{x,y};if (!banks[index(c)]) continue;
            auto& tile=*game.stage.at(c);saved.push_back({c,tile});
            if (channel[index(c)]) {tile={TileKind::ShallowWater};river.channel.push_back(c);}
            else {if (tile.kind==TileKind::Wall) tile={TileKind::Grass};if (walkable(tile)) river.banks.push_back(c);}
        }
        // The centerline is the sole intentional cycle. Wider pockets drain into
        // it through a BFS forest, never point at a nearest segment across land.
        std::vector<std::uint8_t> seen(allowed.size());std::vector<Cell> queue=path;
        for (std::size_t i=0;i<path.size();++i) {seen[index(path[i])]=1;game.stage.at(path[i])->current=flow_code(path[(i+1)%path.size()]-path[i]);}
        for (std::size_t i=0;i<queue.size();++i) for (Cell d:sides) {
            const Cell c=queue[i]+d;if (!game.stage.in_bounds(c) || !channel[index(c)] || seen[index(c)]) continue;
            seen[index(c)]=1;game.stage.at(c)->current=flow_code(queue[i]-c);queue.push_back(c);
        }
        if (queue.size()!=river.channel.size() || !generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
            for (const auto& old:saved) *game.stage.at(old.cell)=old.tile;
            component_result(&plan.report,form,"Rolled back: disconnected current or required route/lock changed");continue;
        }
        component_result(&plan.report,form,"Closed current; side channels feed the loop",river.channel);
        if (form.record>=0) {
            auto& child=plan.report.components[static_cast<std::size_t>(form.record)];
            child.guide=path;child.guide_cell_centers=true;
        }
        component_result(&plan.report,style,"Circulating channel built; shallow walking crossings retained",river.banks);
        auto& decision=plan.report.features.back();decision.outcome=GenerationOutcome::Built;
        decision.variant=std::string("Circulating river / ")+sizes[form.value].name;
        decision.reason="Intentional closed current; later population places rideable supports";
        Cell low{plan.width,plan.height},high{};
        for (Cell c:river.channel) {plan.protected_cells[index(c)]=1;low.x=std::min(low.x,c.x);low.y=std::min(low.y,c.y);high.x=std::max(high.x,c.x+1);high.y=std::max(high.y,c.y+1);}
        decision.regions.push_back({low,high});plan.rivers.push_back(std::move(river));return true;
    }
    component_result(&plan.report,style,"No eligible closed current survived carving");
    auto& decision=plan.report.features.back();decision.outcome=GenerationOutcome::Failed;decision.reason="Circulating route attempts exhausted; see child reasons";return false;
}
