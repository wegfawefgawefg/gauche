#include "streetlights.hpp"
#include "../props/interaction.hpp"
#include "../props/streetlamp.hpp"
#include <cstdlib>

void place_streetlights(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (room.role!=RoomRole::AssemblyLine && room.role!=RoomRole::Workfront &&
        room.role!=RoomRole::FreightSiding && room.role!=RoomRole::ScrapYard) return;
    if (random_u32(game)%3!=0) return;
    for (Cell offset:{Cell{4,3},Cell{-4,3},Cell{4,-3},Cell{-4,-3}}) {
        const Cell cell=room.center+offset;
        bool clear=true;
        // The standing base cannot close a route. Player-triggered wreckage is
        // cuttable; the fall itself refuses quest fixtures and occupied cover.
        for (int y=-1;y<=1 && clear;++y) for (int x=-1;x<=1 && clear;++x) {
            const Cell p=cell+Cell{x,y};const Tile* tile=game.stage.at(p);
            if (!tile || !streetlamp_ground(tile->kind) || !walkable(*tile) || plan.protected_cell(p) ||
                tile->prop.kind!=PropKind::None || entity_at(game,p,false)>=0 ||
                std::abs(p.x-room.center.x)>=room.half_width || std::abs(p.y-room.center.y)>=room.half_height) clear=false;
        }
        if (!clear) continue;
        place_prop(game.stage,cell,PropKind::StreetLamp);return;
    }
}
