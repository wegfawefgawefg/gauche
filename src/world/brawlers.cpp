#include "brawlers.hpp"

void place_brawlers(Game& game,const FloorPlan& plan,RoomSupplies& budget) {
    if ((!ice_floor(game.run.floor) && !industrial_floor(game.run.floor)) || plan.rooms.empty()) return;
    const auto first=random_u32(game)%plan.rooms.size();
    const EntityKind kind=ice_floor(game.run.floor) ? EntityKind::FrostGoblin : EntityKind::PipeGuard;
    int remaining=3+(game.run.floor-1)%4;
    // Use the reserved share after machinery placement so a fighter cannot
    // reject a whole authored installation. One per room keeps approaches legible.
    for (std::size_t index=0;index<plan.rooms.size() && remaining>0;++index) {
        const RoomPlan& room=plan.rooms[(first+index)%plan.rooms.size()];
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit ||
            room.role==RoomRole::Shrine || room.role==RoomRole::Secret) continue;
        if (get_entity(game,spawn_room_enemy(game,room,kind,1,budget))) --remaining;
    }
}
