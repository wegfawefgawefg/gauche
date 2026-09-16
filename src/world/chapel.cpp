#include "chapel.hpp"
#include "../props/interaction.hpp"
#include "../props/candle.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>
#include <cstdlib>

namespace {

bool place(Game& game, const FloorPlan& plan, const RoomPlan& room, Cell offset,
           PropKind kind, std::uint8_t variant = 0) {
    const Cell cell = room.center+offset;
    Tile* tile = game.stage.at(cell);
    const bool blocking = prop_spec(kind).blocking;
    if (!tile || std::abs(offset.x) >= room.half_width || std::abs(offset.y) >= room.half_height ||
        (blocking && plan.protected_cell(cell)) || !walkable(*tile) || surface_wet(*tile) ||
        tile->prop.kind != PropKind::None || entity_at(game,cell,false) >= 0 ||
        distance(cell,game.run.spawn) < 5) return false;
    // Leave the four door approaches and central crossing free of furniture.
    if (blocking && (std::abs(offset.x)<=1 || std::abs(offset.y)<=1)) return false;
    if (!place_prop(game.stage,cell,kind,variant)) return false;
    if (blocking && !floor_reachable(game)) { tile->prop={}; return false; }
    return true;
}

} // namespace

// FURNISHINGS: Broken seating and a reversed apse vary the same usable aisle plan.
// Every blocker stays outside the central/side aisles; props never bury actors or loot.
void place_chapel_props(Game& game, const FloorPlan& plan, const RoomPlan& room) {
    const bool ruined = random_u32(game)%3 == 0;
    const int end = room.mirrored ? 1 : -1;
    const int apse = end*(room.half_height-2);
    bool altar = false;
    for (int x : {-2,2,-3,3})
        if (place(game,plan,room,{x,apse},PropKind::ChapelAltar)) { altar=true; break; }
    // A blocked apse can still have an altar in its opposite side chapel.
    if (!altar)
        for (int x : {2,-2,3,-3})
            if (place(game,plan,room,{x,-apse},PropKind::ChapelAltar)) break;
    for (int y=-room.half_height+3;y<=room.half_height-3;y+=2) {
        if (std::abs(y)<=1) continue;
        for (int x : {-4,-3,3,4}) {
            const bool broken = ruined && random_u32(game)%3 == 0;
            place(game,plan,room,{x,y},broken ? PropKind::ChapelPewBroken : PropKind::ChapelPew);
        }
    }
    // Rugs can share protected walking routes; they never block movement.
    for (int y=-room.half_height+2;y<room.half_height-1;++y)
        if (!ruined || (y+room.half_height)%3 != 0)
            place(game,plan,room,{0,y},PropKind::ChapelRunner);
    int candles=0;
    for (Cell offset : {Cell{-4,apse},{4,apse},{-5,2},{5,2},{-5,-2},{5,-2}}) {
        if (place(game,plan,room,offset,PropKind::Candle)) {
            ++candles;
            // Some wicks await the keeper or the player's flame; preserve real fuel.
            if (ruined && candles%2==0) douse_candle(game,room.center+offset);
            place(game,plan,room,offset+Cell{0,1},PropKind::ChapelWax);
        }
    }
    for (int x : {-5,5}) place(game,plan,room,{x,-apse},PropKind::ChapelUrn);
    for (int x : {room.half_width-2,2-room.half_width})
        if (place(game,plan,room,{x,apse},PropKind::CandleCabinet)) break;
}
