#include "root_growth.hpp"
#include "generation_trace.hpp"
#include "growth_paths.hpp"
#include "growth_carving.hpp"
#include "raster.hpp"
#include "components.hpp"
#include "terrain_material.hpp"
#include "../scenery/hollow_tree.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
bool root_site(const Game& game,const FloorPlan& plan,Cell cell) {
    const auto* tile=game.stage.at(cell);
    if (!tile || cell.x<=0 || cell.y<=0 || cell.x>=plan.width-1 || cell.y>=plan.height-1) return false;
    if (tile->contents!=ItemKind::None || tile->prop.kind!=PropKind::None) return false;
    if (tile->kind==TileKind::Wall && tile->break_rule==BreakRule::Unbreakable) return false;
    if (tile->kind!=TileKind::Wall && tile->kind!=TileKind::Empty && tile->kind!=TileKind::Grass && tile->kind!=TileKind::Ruin) return false;
    for (int index:{0,plan.exit_room,plan.objective_room,plan.secret_room}) {
        if (index<0) continue;
        const auto& room=plan.rooms[static_cast<std::size_t>(index)];
        if (std::abs(cell.x-room.center.x)<=room.half_width+1 && std::abs(cell.y-room.center.y)<=room.half_height+1) return false;
    }
    if (distance(cell,plan.door)<4) return false;
    for (const auto& tree:plan.giant_trees) {
        if (tree_ellipse(tree.canopy,cell,3)) return false;
        for (Cell mouth:tree.entrances) if (distance(cell,mouth)<3) return false;
    }
    // Existing overhead supports and entrances keep their construction; growth
    // may join their wood, but cannot fill the walk-under strip.
    for (const auto& roof:game.stage.roofs) if (roof.kind!=RoofKind::HollowTree && roof_covers(roof,cell)) return false;
    return true;
}
Cell scale(Cell c,int n) {return {c.x*n,c.y*n};}
}

void grow_giant_roots(Game& game,FloorPlan& plan,GenerationTrace* trace) {
    if (plan.giant_trees.empty()) return;
    const auto routes=generation_walking_routes(game,plan);
    std::vector<std::uint8_t> eligible(game.stage.tiles.size(),0);
    for (int y=0;y<plan.height;++y) for (int x=0;x<plan.width;++x)
        eligible[static_cast<std::size_t>(y*plan.width+x)]=root_site(game,plan,{x,y});
    bool changed_world=false;
    for (const auto& tree:plan.giant_trees) {
        const Cell center=tree.canopy.start+Cell{tree.canopy.length/2,tree.canopy.width/2};
        const WeightedComponent reaches[]{{0,"Compact root system",2},{1,"Wandering roots",4},{2,"Sprawling roots",game.run.floor==1 ? 1U : 5U}};
        const auto reach=roll_component(game,&plan.report,GenerationFeature::GiantTree,-1,"Outward roots",center,reaches);
        if (!reach.value) {component_area(&plan.report,reach,"Existing shell and inner roots only");continue;}
        constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
        WeightedComponent headings[]{{0,"East",1},{1,"South",1},{2,"West",1},{3,"North",1}};
        const int stems=2+static_cast<int>(random_u32(game)%3);
        std::vector<Cell> all_changed;
        for (int stem=0;stem<stems;++stem) {
            const auto attachment=roll_component(game,&plan.report,GenerationFeature::GiantTree,reach.record,"Root stem",center,headings);
            headings[attachment.value].weight=0;
            const Cell heading=directions[attachment.value],side{-heading.y,heading.x};
            const int radius=heading.x ? tree.canopy.length/2 : tree.canopy.width/2;
            std::vector<Cell> anchors;
            for (int j=-6;j<=6;++j) for (int r=radius-2;r<=radius;++r) {
                const Cell cell=center+scale(heading,r)+scale(side,j);const auto* tile=game.stage.at(cell);
                if (tile && tile->kind==TileKind::Wall && tile->material==TileMaterial::Root && eligible[static_cast<std::size_t>(cell.y*plan.width+cell.x)]) anchors.push_back(cell);
            }
            if (anchors.empty()) {component_result(&plan.report,attachment,"No available root shell attachment");continue;}
            const Cell origin=anchors[random_u32(game)%anchors.size()];
            component_result(&plan.report,attachment,"Attached to existing shell",std::array{origin});
            const auto paths=branching_paths(game,origin,heading,{reach.value==1 ? 3 : 5,reach.value==1 ? 5 : 8,4,8});
            std::vector<int> records(paths.size(),-1);std::vector<std::vector<Cell>> reached(paths.size());
            for (std::size_t n=0;n<paths.size();++n) {
                const auto& path=paths[n];
                const int parent=path.parent<0 ? attachment.record : records[static_cast<std::size_t>(path.parent)];
                const WeightedComponent widths[]{{0,"Fine root",3},{1,"Raised root",5},{2,"Heavy buttress",reach.value==2 ? 3U : 1U}};
                const auto branch=roll_component(game,&plan.report,GenerationFeature::GiantTree,parent,"Root branch",path.points.front(),widths);
                const GenerationStep step{trace,game,plan,"Root branch",GenerationFeature::GiantTree,branch.record};
                records[n]=branch.record;
                if (branch.record>=0) plan.report.components[static_cast<std::size_t>(branch.record)].guide=path.points;
                if (path.parent>=0) {
                    const auto& prior=reached[static_cast<std::size_t>(path.parent)];
                    if (std::find(prior.begin(),prior.end(),path.points.front())==prior.end()) {
                        component_area(&plan.report,branch,"Fork beyond surviving parent root");continue;
                    }
                }
                std::vector<Cell> proposed;bool truncated=false;
                for (std::size_t i=1;i<path.points.size();++i) {
                    const int radius_here=std::max(0,branch.value-(i*2>path.points.size() ? 1 : 0));
                    auto segment=raster_line(path.points[i-1],path.points[i],radius_here,plan.width,plan.height);
                    // Thin diagonal roots need their stair-step connection in tile space.
                    if (!radius_here) for (std::size_t k=1;k<segment.cells.size();++k) {
                        const Cell a=segment.cells[k-1],b=segment.cells[k];
                        if (a.x!=b.x && a.y!=b.y) proposed.push_back({b.x,a.y});
                    }
                    truncated|=segment.truncated;proposed.insert(proposed.end(),segment.cells.begin(),segment.cells.end());
                }
                if (truncated) {component_area(&plan.report,branch,"Raster budget exceeded");continue;}
                auto connected=connected_raster(path.points.front(),proposed,eligible,plan.width,plan.height);
                if (connected.empty()) {component_area(&plan.report,branch,"No connected eligible terrain");continue;}
                struct Saved {Cell cell;Tile tile;};std::vector<Saved> saved;std::vector<Cell> changed,skipped;
                int mineral=0,raised=0,walking=0;
                for (Cell cell:connected) {
                    const auto index=static_cast<std::size_t>(cell.y*plan.width+cell.x);auto* tile=game.stage.at(cell);
                    if (routes[index]) {++walking;skipped.push_back(cell);continue;}
                    if (tile->kind==TileKind::Wall && tile->material==TileMaterial::Root) continue;
                    if (tile->kind==TileKind::Wall) {if (!wooden_terrain(*tile)) ++mineral;}
                    else ++raised;
                    saved.push_back({cell,*tile});*tile=wood_tile(TileMaterial::Root);changed.push_back(cell);
                }
                for (Cell cell:proposed) if (game.stage.in_bounds(cell) && !eligible[static_cast<std::size_t>(cell.y*plan.width+cell.x)] && std::find(skipped.begin(),skipped.end(),cell)==skipped.end()) skipped.push_back(cell);
                if (branch.record>=0) plan.report.components[static_cast<std::size_t>(branch.record)].rejected_cells=skipped;
                if (!generation_lock_intact(game,plan) || !generation_exit_reachable(game,plan)) {
                    for (const auto& old:saved) *game.stage.at(old.cell)=old.tile;
                    component_area(&plan.report,branch,"Rolled back: required route or lock changed");continue;
                }
                reached[n]=std::move(connected);
                all_changed.insert(all_changed.end(),changed.begin(),changed.end());
                const std::string result=std::to_string(mineral)+" mineral walls rooted; "+std::to_string(raised)+
                    " floor cells raised; "+std::to_string(walking)+" walking-route cells kept open; "+std::to_string(skipped.size())+" skipped cells";
                component_area(&plan.report,branch,result.c_str(),changed);
            }
        }
        changed_world|=!all_changed.empty();
        component_area(&plan.report,reach,all_changed.empty() ? "No terrain converted; inspect stem/branch results" : "Actual root material; cut or burn to open",all_changed);
    }
    if (changed_world) for (std::size_t i=0;i<routes.size();++i)
        if (routes[i]) plan.protected_cells[i]=1;
}
