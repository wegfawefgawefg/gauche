#include "chapel.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"

#include <cstdlib>

namespace {

bool place(Game& game, const FloorPlan& plan, const RoomPlan& room, Cell offset, PropKind kind) {
    const Cell cell = room.center+offset;
    Tile* tile = game.stage.at(cell);
    if (!tile || std::abs(offset.x) >= room.half_width || std::abs(offset.y) >= room.half_height ||
        plan.protected_cell(cell) || !walkable(*tile) || surface_wet(*tile) ||
        tile->prop.kind != PropKind::None || entity_at(game,cell,false) >= 0 ||
        distance(cell,game.run.spawn) < 5) return false;
    if (prop_spec(kind).blocking)
        for (Cell side : {Cell{1,0},{0,1},{-1,0},{0,-1}})
            if (!walkable(game.stage.at_or_border(cell+side))) return false;
    return place_prop(game.stage,cell,kind);
}

} // namespace

// AISLES: A small arrangement in the alcoves; mandatory crossings stay clear.
void place_chapel_props(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    int candles = 0;
    for (Cell offset : {Cell{-4,-2},{4,-2},{-4,2},{4,2},{-2,-2},{2,-2},{-2,2},{2,2}}) {
        if (candles >= 3) break;
        if (place(game,plan,room,offset,PropKind::Candle)) ++candles;
    }
    for (int y=-room.half_height+1;y<room.half_height;++y) {
        bool placed = false;
        for (int x=-room.half_width+1;x<room.half_width;++x)
            if (place(game,plan,room,{x,y},PropKind::CandleCabinet)) { placed = true; break; }
        if (placed) break;
    }
}
