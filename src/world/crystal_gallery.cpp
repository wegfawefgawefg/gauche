#include "crystal_gallery.hpp"
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
