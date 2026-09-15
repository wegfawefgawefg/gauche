#include "rivet_post.hpp"
#include "../props/interaction.hpp"
#include <array>

namespace {
bool clear(const Game& game,const FloorPlan& plan,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || tile->prop.kind!=PropKind::None || tile->kind==TileKind::Lava ||
        tile->kind==TileKind::Water || tile->kind==TileKind::ShallowWater || plan.protected_cell(cell)) return false;
    for (const Entity& actor:game.entities)
        if (actor.kind!=EntityKind::None && distance(actor.cell,cell)<=1) return false;
    return true;
}
}
bool populate_rivet_post(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    // Optional side alcove: three bars with open ends, outside the reserved route.
    for (Cell outward:{Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}})
    for (int lateral:{2,-2,0,3,-3}) {
        const Cell side{-outward.y,outward.x};
        const Cell center=room.center+Cell{outward.x*4+side.x*lateral,outward.y*4+side.y*lateral};
        const Cell gunner=center+outward;
        const std::array cells{center-side,center,center+side,gunner};
        bool okay=true;
        for (Cell cell:cells)
            if (std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height || !clear(game,plan,cell)) okay=false;
        if (!okay) continue;
        Entity* actor=get_entity(game,spawn_entity(game,EntityKind::RivetGunner,gunner));
        if (!actor) return false;
        actor->facing={-outward.x,-outward.y};
        for (Cell cell:{center-side,center,center+side}) place_prop(game.stage,cell,PropKind::Grate,side.x==0 ? 1 : 0);
        return true;
    }
    return false;
}
