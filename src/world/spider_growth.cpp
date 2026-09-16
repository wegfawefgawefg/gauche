#include "spider_growth.hpp"
#include "growth_paths.hpp"
#include "growth_carving.hpp"
#include "raster.hpp"
#include "components.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
bool allowed(const Game& game,const FloorPlan& plan,Cell cell) {
    const auto* tile=game.stage.at(cell);
    if (!tile || cell.x<=1 || cell.y<=1 || cell.x>=plan.width-2 || cell.y>=plan.height-2) return false;
    for (int room:{0,plan.exit_room,plan.objective_room,plan.secret_room}) {
        if (room<0) continue;
        const auto& r=plan.rooms[static_cast<std::size_t>(room)];
        if (std::abs(cell.x-r.center.x)<=r.half_width+2 && std::abs(cell.y-r.center.y)<=r.half_height+2) return false;
    }
    if (distance(cell,plan.door)<4 || tile->contents!=ItemKind::None || prop_blocks(tile->prop)) return false;
    if (tile->kind==TileKind::Wall)
        return tile->break_rule!=BreakRule::Unbreakable && tile->material!=TileMaterial::Root && tile->prop.kind==PropKind::None;
    return tile->kind==TileKind::Grass || tile->kind==TileKind::Empty;
}

}

void grow_spider_habitats(Game& game,FloorPlan& plan) {
    if (plan.spider_caves.empty()) return;
    std::vector<std::uint8_t> mask(game.stage.tiles.size());
    for (int y=0;y<plan.height;++y) for (int x=0;x<plan.width;++x)
        mask[static_cast<std::size_t>(y*plan.width+x)]=allowed(game,plan,{x,y});
    for (const auto& cave:plan.spider_caves) {
        const Cell origin=plan.rooms[static_cast<std::size_t>(cave.rooms[random_u32(game)%cave.rooms.size()])].center;
        const bool early=biome_stage(game.run.floor)==1;
        const WeightedComponent modes[]{{0,"Contained cave",3},{1,"Outlying burrows",4},{2,"Branching infestation",early ? 1U : 5U}};
        const auto mode=roll_component(game,&plan.report,GenerationFeature::SpiderCave,-1,"Habitat reach",origin,modes);
        if (!mode.value) {component_result(&plan.report,mode,"No outward growth");continue;}
        constexpr Cell directions[]{{1,0},{0,1},{-1,0},{0,-1}};
        const GrowthSettings settings{mode.value==1 ? 3 : 6,mode.value==1 ? 4 : 7,4,8};
        const auto paths=branching_paths(game,origin,directions[random_u32(game)%4],settings);
        std::vector<int> records(paths.size(),-1);std::vector<std::vector<Cell>> reached(paths.size());
        std::vector<Cell> all_opened;
        for (std::size_t n=0;n<paths.size();++n) {
            const auto& path=paths[n];
            const int parent=path.parent<0 ? mode.record : records[static_cast<std::size_t>(path.parent)];
            const WeightedComponent shapes[]{{1,"Narrow vein",4},{2,"Broad web gallery",3},{3,"Brood chamber",3}};
            const auto branch=roll_component(game,&plan.report,GenerationFeature::SpiderCave,parent,"Habitat branch",path.points.front(),shapes);
            records[n]=branch.record;
            if (branch.record>=0) plan.report.components[static_cast<std::size_t>(branch.record)].guide=path.points;
            if (path.parent>=0 && reached[static_cast<std::size_t>(path.parent)].empty()) {
                component_result(&plan.report,branch,"Parent branch rejected");continue;
            }
            if (path.parent>=0 && std::find(reached[static_cast<std::size_t>(path.parent)].begin(),
                reached[static_cast<std::size_t>(path.parent)].end(),path.points.front())==reached[static_cast<std::size_t>(path.parent)].end()) {
                component_result(&plan.report,branch,"Fork lies beyond the surviving parent path");continue;
            }
            std::vector<Cell> proposed;bool truncated=false;
            for (std::size_t i=1;i<path.points.size();++i) {
                const int width=branch.value==2 ? 2+static_cast<int>(random_u32(game)%2) : 1+static_cast<int>(random_u32(game)%2);
                auto segment=raster_line(path.points[i-1],path.points[i],width,plan.width,plan.height);
                truncated|=segment.truncated;proposed.insert(proposed.end(),segment.cells.begin(),segment.cells.end());
            }
            if (branch.value==3) {
                const Cell tip=path.points.back();
                const int a=3+static_cast<int>(random_u32(game)%4),b=3+static_cast<int>(random_u32(game)%4);
                const std::array polygon{tip+Cell{-a,-2},tip+Cell{-2,-b},tip+Cell{a,-b+2},tip+Cell{a+1,1},tip+Cell{2,b},tip+Cell{-a,b-1}};
                auto chamber=raster_polygon(polygon,plan.width,plan.height);
                truncated|=chamber.truncated;proposed.insert(proposed.end(),chamber.cells.begin(),chamber.cells.end());
            }
            if (truncated) {component_result(&plan.report,branch,"Raster budget exceeded");continue;}
            auto ground=connected_raster(path.points.front(),proposed,mask,plan.width,plan.height);
            struct Saved {Cell cell;Tile tile;};std::vector<Saved> saved;std::vector<Cell> opened;
            for (Cell cell:ground) if (auto* tile=game.stage.at(cell);tile->kind==TileKind::Wall) {
                saved.push_back({cell,*tile});*tile={TileKind::Empty};opened.push_back(cell);
            }
            if (ground.size()<12 || !generation_lock_intact(game,plan)) {
                for (const auto& old:saved) *game.stage.at(old.cell)=old.tile;
                component_result(&plan.report,branch,ground.size()<12 ? "Blocked by protected terrain or hazards" : "Would bypass exit lock or disconnect objective");continue;
            }
            reached[n]=ground;
            // Population belongs to the surviving connected footprint, including
            // invaded existing floors. Do not fence it off as another room reservation.
            plan.spider_growth.push_back({std::move(ground),branch.record,static_cast<int>(opened.size())});
            all_opened.insert(all_opened.end(),opened.begin(),opened.end());
            component_result(&plan.report,branch,"Connected habitat; population pending",plan.spider_growth.back().ground);
        }
        component_result(&plan.report,mode,all_opened.empty() ? "No new walls opened; inspect surviving floor invasion" : "Outward habitat carved",all_opened);
    }
}
