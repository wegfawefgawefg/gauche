#pragma once
#include "route.hpp"
#include <utility>

// Generation coordinates only. Completed terrain/actors retain world cells.
inline Cell turn_cell(Cell cell,int turns) {
    for (int i=0;i<(turns&3);++i) cell={-cell.y,cell.x};
    return cell;
}
inline Cell room_offset(const RoomPlan& room,Cell local) {
    if (room.mirrored) local.x=-local.x;
    return turn_cell(local,room.turns);
}
inline Cell room_local(const RoomPlan& room,Cell offset) {
    offset=turn_cell(offset,4-room.turns);
    if (room.mirrored) offset.x=-offset.x;
    return offset;
}
inline RoomPlan unturned_room(RoomPlan room) {
    if (room.turns&1) std::swap(room.half_width,room.half_height);
    room.turns=0;
    return room;
}
inline bool socket_room(const Game& game,const RoomPlan& room) {
    if (room.shape>=RoomShape::ChapelNave) return false;
    if (forest_floor(game.run.floor)) return true;
    if (!ice_floor(game.run.floor)) return false;
    return room.role==RoomRole::Reservoir || room.role==RoomRole::IceQuarry ||
        room.role==RoomRole::EchoTunnel || room.role==RoomRole::CliffPath;
}
