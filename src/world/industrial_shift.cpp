#include "industrial_shift.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

namespace {
Cell scale(Cell cell,int n) {return {cell.x*n,cell.y*n};}
Cell origin(const IndustrialLink& link) {return link.load-scale(link.across,4);}
}
void carve_industrial_shift(Game& game,FloorPlan& plan,const IndustrialLink& link) {
    const Cell start=origin(link),side{-link.along.y,link.along.x};
    // Beside the dry service lane, away from the automatic cutter's seam. The
    // protected central crossing remains open around this finite nine-cell face.
    for (int depth=-1;depth<=6;++depth) for (int lane=-2;lane<=2;++lane) {
        const Cell cell=start+scale(link.along,depth)+scale(side,lane);
        Tile* tile=game.stage.at(cell);if (!tile) continue;
        *tile={TileKind::Ruin};
        if (depth>=4 && lane>=-1 && lane<=1) {
            *tile={TileKind::Wall,75,0,75,BreakRule::DigRequired,1};
            tile->contents=ItemKind::CoalLump;tile->content_count=1;
        }
        plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
    }
}
void populate_industrial_shift(Game& game,const IndustrialLink& link,Handle tank) {
    const Cell start=origin(link),direction=link.along,side{-direction.y,direction.x};
    const Cell hauler_cell=link.load-scale(link.across,2)-direction;
    const Cell places[]{start,start+direction-side,start+direction,start+direction+side,hauler_cell};
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return;
    for (Cell cell:places) {
        const Tile& tile=game.stage.at_or_border(cell);
        if (!walkable(tile) || tile.kind==TileKind::Lava || entity_at(game,cell,false)>=0 || distance(cell,game.run.spawn)<8) return;
    }
    const Handle leader=spawn_entity(game,EntityKind::ShiftForeman,start);
    auto& foreman=*get_entity(game,leader);foreman.entity_a=leader;foreman.facing=direction;
    for (int lane=-1;lane<=1;++lane) {
        auto& worker=*get_entity(game,spawn_entity(game,EntityKind::Pickhand,start+direction+scale(side,lane)));
        worker.entity_a=leader;worker.counter_b=lane;worker.facing=direction;worker.move_wait=(lane+1)*3;
    }
    const Handle hauler=spawn_entity(game,EntityKind::Pickhand,hauler_cell);
    get_entity(game,hauler)->entity_a=leader;get_entity(game,hauler)->facing=direction;
    game.industrial_shifts.push_back({tank,leader,hauler,start,direction});
    const Cell guard_cell=start-direction-side;
    if ((game.run.floor-1)%4>=1 && entity_at(game,guard_cell,false)<0) {
        if (auto* guard=get_entity(game,spawn_entity(game,EntityKind::Strikebreaker,guard_cell))) {
            guard->entity_a=leader;guard->facing=direction;
        }
    }
    // Signs of a shared shift, outside the carrying route and digging lanes.
    for (int offset:{-2,2}) {
        const Cell cell=start-direction+scale(side,offset);
        if (walkable(game.stage.at_or_border(cell)) && entity_at(game,cell,false)<0)
            place_prop(game.stage,cell,PropKind::ScrapBin);
    }
}
