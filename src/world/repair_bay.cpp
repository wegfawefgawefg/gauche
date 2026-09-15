#include "repair_bay.hpp"
#include "../props/interaction.hpp"
#include <vector>

namespace {
bool free_cell(const Game& game,const FloorPlan& plan,const RoomPlan& room,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && walkable(*tile) && tile->kind!=TileKind::Lava &&
        tile->prop.kind==PropKind::None && !plan.protected_cell(cell) &&
        std::abs(cell.x-room.center.x)<room.half_width &&
        std::abs(cell.y-room.center.y)<room.half_height &&
        distance(cell,game.run.spawn)>=5 && entity_at(game,cell,false)<0;
}
}
Handle populate_repair_bay(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    // Offset wet work area beside a dry bank. Grate benches sit on the opposite
    // side of the room. All pieces are optional and outside reserved routes.
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](int x,int y) {return room.center+Cell{x*sign,y};};
        std::vector<Cell> pool;
        for (int y=-4;y<=-2;++y) for (int x=2;x<=4;++x) pool.push_back(at(x,y));
        const Cell worker=at(5,-3),sink=at(3,-5);
        std::vector<Cell> footprint=pool;
        for (Cell cell:{worker,sink,at(-4,-3),at(-4,-2)}) footprint.push_back(cell);
        bool okay=true;
        for (Cell cell:footprint) if (!free_cell(game,plan,room,cell)) {okay=false;break;}
        if (!okay) continue;
        const Handle handle=spawn_entity(game,EntityKind::ArcWelder,worker);
        Entity* actor=get_entity(game,handle);
        if (!actor) return {};
        actor->facing={-sign,0};
        for (Cell cell:pool) *game.stage.at(cell)={TileKind::ShallowWater,0,0};
        place_prop(game.stage,sink,PropKind::GroundingSpike);
        for (Cell cell:{at(-4,-3),at(-4,-2)}) place_prop(game.stage,cell,PropKind::Grate,1);
        return handle;
    }
    return {};
}
