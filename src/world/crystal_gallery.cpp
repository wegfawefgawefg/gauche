#include "crystal_gallery.hpp"
#include "ice_terrain.hpp"
#include "../props/interaction.hpp"
#include "../entities/shard_colony.hpp"
#include "../surfaces/interaction.hpp"

#include <cstdlib>

Handle populate_crystal_gallery(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    // FORMATION: Try mirrored triangles in clear alcoves, never on a reserved crossing.
    for (int side : {1,-1})
        for (int turn : {1,-1}) {
            const std::array<Cell,3> cells{{room.center+Cell{-2,2*turn},
                room.center+Cell{2,2*turn},room.center+Cell{2*side,-2*turn}}};
            bool suitable = true;
            for (Cell cell : cells) {
                const Tile* tile = game.stage.at(cell);
                if (!tile || plan.protected_cell(cell) || !walkable(*tile) || surface_wet(*tile) ||
                    distance(cell,game.run.spawn)<5 ||
                    std::abs(cell.x-room.center.x)>=room.half_width ||
                    std::abs(cell.y-room.center.y)>=room.half_height) suitable = false;
                for (const Entity& actor : game.entities)
                    if ((actor.kind == EntityKind::Door || actor.kind == EntityKind::Exit ||
                        actor.kind == EntityKind::Key || actor.kind == EntityKind::Switch) && distance(actor.cell,cell)<=2)
                        suitable = false;
            }
            if (!suitable) continue;
            const Handle colony = spawn_shard_colony(game,cells);
            if (get_entity(game,colony)) return colony;
        }
    return {};
}

void place_crystal_vein(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    if (!ice_floor(game.run.floor) || (room.role!=RoomRole::CrystalGallery &&
        room.role!=RoomRole::EchoTunnel && room.role!=RoomRole::IceQuarry)) return;
    const int length=room.role==RoomRole::CrystalGallery ? 5 : 3;
    // ALCOVES: One contiguous vein, never on a mandatory path or on top of actors.
    // The ordinary punch also breaks it, so the fork cannot become a required key.
    for (Cell axis : {Cell{1,0},Cell{0,1}})
        for (int y=-room.half_height+2;y<room.half_height-1;++y)
            for (int x=-room.half_width+2;x<room.half_width-1;++x) {
                const Cell start=room.center+Cell{x,y};
                bool clear=true;
                for (int i=0;i<length && clear;++i) {
                    const Cell cell=start+Cell{axis.x*i,axis.y*i};
                    const Tile* tile=game.stage.at(cell);
                    clear=tile && walkable(*tile) && !surface_wet(*tile) &&
                        tile->prop.kind==PropKind::None && !plan.protected_cell(cell) &&
                        entity_at(game,cell,false)<0 && distance(cell,game.run.spawn)>5 &&
                        std::abs(cell.x-room.center.x)<room.half_width-1 &&
                        std::abs(cell.y-room.center.y)<room.half_height-1;
                }
                if (!clear) continue;
                for (int i=0;i<length;++i)
                    place_prop(game.stage,start+Cell{axis.x*i,axis.y*i},PropKind::CrystalGrowth);
                return;
            }
}
