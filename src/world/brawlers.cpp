#include "brawlers.hpp"
#include "ranged_groups.hpp"
#include "../entities/attacks.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};

bool combat_room(RoomRole role) {
    return role!=RoomRole::Entrance && role!=RoomRole::Exit && role!=RoomRole::Shrine &&
        role!=RoomRole::Secret && role!=RoomRole::Shelter && role!=RoomRole::Bathhouse &&
        role!=RoomRole::FishingHut;
}
bool open_ground(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && walkable(*tile) && tile->kind!=TileKind::Lava &&
        tile->kind!=TileKind::IceHole && tile->kind!=TileKind::Spring;
}
bool approach_space(const Game& game,Cell cell) {
    if (!open_ground(game,cell) || distance(cell,game.run.spawn)<=7 || entity_at(game,cell,false)>=0) return false;
    int exits=0;
    for (Cell side:sides) exits+=open_ground(game,cell+side) && entity_at(game,cell+side,true)<0;
    return exits>=3;
}
int occupants(const Game& game,const RoomPlan& room) {
    int count=0;
    for (const Entity& e:game.entities)
        if (e.health>0 && e.impassable && !e.hard_blocker &&
            std::abs(e.cell.x-room.center.x)<=room.half_width &&
            std::abs(e.cell.y-room.center.y)<=room.half_height) ++count;
    return count;
}
}

void place_brawlers(Game& game,const FloorPlan& plan,RoomSupplies& budget) {
    if ((!ice_floor(game.run.floor) && !industrial_floor(game.run.floor)) || plan.rooms.empty() || budget.threat<=0) return;
    const EntityKind kind=ice_floor(game.run.floor) ? EntityKind::FrostGoblin : EntityKind::PipeGuard;
    std::vector<std::size_t> rooms;
    for (std::size_t i=0;i<plan.rooms.size();++i)
        if (combat_room(plan.rooms[i].role)) rooms.push_back(i);
    for (std::size_t i=rooms.size();i>1;--i) std::swap(rooms[i-1],rooms[random_u32(game)%i]);
    place_ranged_group(game,plan,rooms,budget);
    // Specialists and installations get their spaces first. Quiet refuges remain
    // quiet; roomy encounters may gain a pair, or a late-biome trio, not a lone
    // fighter stamped into every room. Failed placements do not spend the budget.
    const int round=(game.run.floor-1)%4;
    for (const auto index:rooms) {
        if (budget.threat<=0) break;
        const RoomPlan& room=plan.rooms[index];
        auto choices=room_spaces(game,room,kind);
        if (choices.size()<25) continue;
        std::erase_if(choices,[&](Cell cell){return !approach_space(game,cell);});
        if (choices.empty()) continue;
        const auto roll=random_u32(game)%4;
        int count=std::min({budget.threat,4-occupants(game,room),roll==0 ? 1 : round>=2 && roll==3 ? 3 : 2});
        Cell anchor=choices[random_u32(game)%choices.size()];
        while (count-->0) {
            if (get_entity(game,spawn_entity(game,kind,anchor))==nullptr) break;
            --budget.threat;
            // Space the group so a single approach never starts on four adjacent
            // attackers; a wall cannot secretly split the proposed group in two.
            std::erase_if(choices,[&](Cell cell){
                const int gap=distance(cell,anchor);
                return gap<2 || gap>4 || !approach_space(game,cell) || !clear_attack_sight(game,anchor,cell);
            });
            if (choices.empty()) break;
            anchor=choices[random_u32(game)%choices.size()];
        }
    }
}
