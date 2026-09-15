#include "assembly.hpp"
#include "ground_items.hpp"
#include "../props/conveyor.hpp"
#include "../props/interaction.hpp"

void place_assembly_belts(Game& game,const FloorPlan& plan) {
    for (const RoomPlan& room:plan.rooms) {
        if (room.role!=RoomRole::AssemblyLine) continue;
        // Two lanes with fixed dry islands at the protected central crossing.
        // One upper run has no power; the supplied crank can drive it by hand.
        for (int lane:{-3,3}) for (int x=-5;x<=5;++x) {
            const Cell cell=room.center+Cell{x,lane};
            Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || plan.protected_cell(cell)) continue;
            place_prop(game.stage,cell,PropKind::Conveyor,lane<0 ? (x>1 ? belt_manual : 0) : 2);
        }
    }
}
void assembly_supplies(Game& game,const RoomPlan& room) {
    if (room.role!=RoomRole::AssemblyLine) return;
    place_ground_item(game,room.center+Cell{-2,0},ItemKind::BeltCrank);
    place_ground_item(game,room.center+Cell{2,0},ItemKind::BrakeShoe);
    for (Cell offset:{Cell{-4,-3},Cell{4,3}}) {
        const Cell cell=room.center+offset;
        if (live_belt(game.stage.at_or_border(cell).prop) && entity_at(game,cell,false)<0)
            place_ground_item(game,cell,ItemKind::CoalLump);
    }
}
