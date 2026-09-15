#include "sluice.hpp"
#include "ice_terrain.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../entities/gate.hpp"
#include "../props/interaction.hpp"
#include <cstdlib>

namespace {
bool chamber_space(const Game& game,const FloorPlan& plan,const RoomPlan& room,Cell center,Cell entrance) {
    for (int y=-2;y<=2;++y)
        for (int x=-2;x<=2;++x) {
            const Cell cell=center+Cell{x,y};
            const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || plan.protected_cell(cell) ||
                tile->prop.kind!=PropKind::None || entity_at(game,cell)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width ||
                std::abs(cell.y-room.center.y)>=room.half_height) return false;
        }
    const Tile* approach=game.stage.at(entrance);
    return approach && walkable(*approach) && entity_at(game,entrance)<0;
}
}

bool place_sluice_chamber(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (!ice_floor(game.run.floor) || room.role!=RoomRole::Reservoir) return false;
    for (int sy:{1,-1}) for (int sx:{-1,1}) {
        const Cell center=room.center+Cell{sx*4,sy*4};
        const Cell gate_cell=center+Cell{0,-sy*2};
        const Cell entrance=center+Cell{0,-sy*3};
        if (!chamber_space(game,plan,room,center,entrance)) continue;
        Entity* gate=get_entity(game,spawn_entity(game,EntityKind::EncounterGate,gate_cell));
        if (!gate) return false;
        configure_timed_gate(*gate);
        // OPTIONAL: A single timed entrance, wholly outside the protected route graph.
        // Keep both sides dry; a player can always wait for the next opening to leave.
        for (int y=-2;y<=2;++y) for (int x=-2;x<=2;++x) {
            const Cell cell=center+Cell{x,y};
            const bool wall=(std::abs(x)==2 || std::abs(y)==2) && cell!=gate_cell;
            *game.stage.at(cell)=wall ? Tile{TileKind::Wall,100,0} : Tile{TileKind::Ruin,0,0};
        }
        place_prop(game.stage,center,PropKind::MaintenanceLocker);
        place_coins(game,center+Cell{1,0},6);
        place_ground_item(game,entrance,ItemKind::EmergencyDoorstop);
        return true;
    }
    return false;
}
