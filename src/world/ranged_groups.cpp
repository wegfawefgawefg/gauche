#include "ranged_groups.hpp"
#include "../entities/attacks.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
bool inside(const RoomPlan& room,Cell cell) {
    return std::abs(cell.x-room.center.x)<=room.half_width &&
           std::abs(cell.y-room.center.y)<=room.half_height;
}
bool dry_space(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    return tile && walkable(*tile) && tile->kind!=TileKind::Lava &&
        tile->kind!=TileKind::Water && tile->kind!=TileKind::ShallowWater &&
        tile->kind!=TileKind::IceHole && tile->kind!=TileKind::Spring &&
        entity_at(game,cell,false)<0;
}
bool approach_space(const Game& game,Cell cell) {
    if (!dry_space(game,cell) || distance(cell,game.run.spawn)<=10) return false;
    int exits=0;
    for (Cell side:sides) exits+=dry_space(game,cell+side);
    return exits>=3;
}
std::optional<Cell> open_lane(const Game& game,const RoomPlan& room,Cell origin) {
    for (Cell direction:sides) {
        const Cell across{-direction.y,direction.x};
        bool clear=true;
        // Three dry approach cells, with a sidestep on either side of the lane.
        // No forced lava/pit dodge, and no firing position tucked into a doorway.
        for (int reach=1;reach<=3;++reach) {
            const Cell cell=origin+Cell{direction.x*reach,direction.y*reach};
            if (!inside(room,cell) || !dry_space(game,cell)) clear=false;
            if (reach>=2 && (!dry_space(game,cell+across) || !dry_space(game,cell-across))) clear=false;
        }
        if (clear) return direction;
    }
    return std::nullopt;
}
int bodies(const Game& game,const RoomPlan& room) {
    return static_cast<int>(std::count_if(game.entities.begin(),game.entities.end(),[&](const Entity& e){
        return e.health>0 && e.impassable && !e.hard_blocker && inside(room,e.cell);
    }));
}
void report_spawn(RoomSupplies& budget,EntityKind kind) {
    if (!budget.report) return;
    auto& count=budget.report->enemies[static_cast<std::size_t>(kind)];
    ++count.attempted;++count.placed;
}
bool place(Game& game,const RoomPlan& room,RoomSupplies& budget,bool reuse) {
    const int cost=reuse ? 1 : 3;
    if (budget.threat<cost || bodies(game,room)>(reuse ? 3 : 2)) return false;
    auto ground=room_spaces(game,room);
    if (ground.size()<25) return false;
    std::erase_if(ground,[&](Cell cell){return !approach_space(game,cell);});
    if (ground.empty()) return false;
    std::vector<Cell> posts;
    if (reuse) {
        for (const Entity& e:game.entities)
            if (e.kind==EntityKind::RivetGunner && e.health>0 && inside(room,e.cell) &&
                distance(e.cell,game.run.spawn)>10) posts.push_back(e.cell);
    } else posts=ground;
    if (posts.empty()) return false;
    const auto start=random_u32(game)%posts.size();
    for (std::size_t i=0;i<posts.size();++i) {
        const Cell post=posts[(start+i)%posts.size()];
        if (!reuse && std::any_of(game.entities.begin(),game.entities.end(),[&](const Entity& e){
            return e.kind==EntityKind::RivetGunner && e.health>0 && distance(e.cell,post)<9;
        })) continue;
        const auto lane=open_lane(game,room,post);
        if (!lane) continue;
        std::vector<Cell> escorts;
        for (Cell cell:ground) {
            const int gap=distance(post,cell);
            if (gap>=3 && gap<=5 && cell.x!=post.x && cell.y!=post.y &&
                clear_attack_sight(game,post,cell)) escorts.push_back(cell);
        }
        if (escorts.empty()) continue;
        const int free=static_cast<int>(std::count_if(game.entities.begin(),game.entities.end(),
            [](const Entity& e){return e.kind==EntityKind::None;}));
        if (free<(reuse ? 1 : 2)) return false;
        const Cell escort=escorts[random_u32(game)%escorts.size()];
        if (!reuse) {
            get_entity(game,spawn_entity(game,EntityKind::RivetGunner,post))->facing=*lane;
            report_spawn(budget,EntityKind::RivetGunner);
        }
        get_entity(game,spawn_entity(game,EntityKind::PipeGuard,escort))->facing=*lane;
        report_spawn(budget,EntityKind::PipeGuard);
        budget.threat-=cost;
        if (budget.report) budget.report->ranged_groups.push_back({post,escort,!reuse});
        return true;
    }
    return false;
}
}

void place_ranged_group(Game& game,const FloorPlan& plan,
                        const std::vector<std::size_t>& rooms,RoomSupplies& budget) {
    if (!industrial_floor(game.run.floor) || budget.threat<1) return;
    for (auto i:rooms) if (place(game,plan.rooms[i],budget,true)) return;
    const int gunners=static_cast<int>(std::count_if(game.entities.begin(),game.entities.end(),
        [](const Entity& e){return e.kind==EntityKind::RivetGunner && e.health>0;}));
    // Specialist rooms can exceed this; the ordinary pass never piles more on.
    if (gunners>=2+(game.run.floor-1)%4/2) return;
    for (auto i:rooms) if (place(game,plan.rooms[i],budget,false)) return;
}
