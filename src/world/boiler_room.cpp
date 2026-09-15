#include "boiler_room.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"

void place_maintenance_locker(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    // ACCESS: Wall-side supplies never occupy a reserved crossing or seal a one-cell aisle.
    for (int y=-room.half_height+1;y<room.half_height;++y)
        for (int x=-room.half_width+1;x<room.half_width;++x) {
            const Cell cell = room.center+Cell{x,y};
            const Tile* tile = game.stage.at(cell);
            if (!tile || plan.protected_cell(cell) || !walkable(*tile) || surface_wet(*tile) ||
                tile->prop.kind != PropKind::None || entity_at(game,cell,false) >= 0) continue;
            bool clear = true;
            for (Cell side : {Cell{1,0},{-1,0},{0,1},{0,-1}})
                if (!walkable(game.stage.at_or_border(cell+side))) clear = false;
            if (clear && place_prop(game.stage,cell,PropKind::MaintenanceLocker)) return;
        }
}
