#include "workfront.hpp"
#include "../entities/mine_crew.hpp"
#include "../props/interaction.hpp"
#include "ground_items.hpp"
#include <algorithm>

void place_workfront_terrain(Game& game,const FloorPlan& plan) {
    if (!industrial_floor(game.run.floor)) return;
    for (const RoomPlan& room:plan.rooms) {
        if (room.role==RoomRole::BlastingAlcove) {
            for (int y=-3;y<=-1;++y) for (int x=2;x<=4;++x) {
                const Cell cell=room.center+Cell{x,y};
                Tile* tile=game.stage.at(cell);
                if (tile && !plan.protected_cell(cell)) *tile={TileKind::Wall,90,0,90,BreakRule::DigRequired,1};
            }
            continue;
        }
        if (room.role!=RoomRole::Workfront) continue;
        // A short mining spur sits beside the protected central route. Excavation
        // reveals ordinary dry cells and leaves all required crossings intact.
        for (int y=-4;y<=-2;++y)
            for (int x=3;x<=5;++x) {
                const Cell cell=room.center+Cell{x,y};
                Tile* tile=game.stage.at(cell);
                if (tile && !plan.protected_cell(cell)) *tile={TileKind::Wall,75,0,75,BreakRule::DigRequired,1};
            }
    }
}

int populate_workfront(Game& game,const RoomPlan& room) {
    if (room.role!=RoomRole::Workfront) return 0;
    // The connected loading shift already fills a workfront at this end.
    for (const auto& shift:game.industrial_shifts) {
        const Entity* leader=get_entity(game,shift.foreman);
        if (!leader || std::abs(leader->cell.x-room.center.x)>=room.half_width ||
            std::abs(leader->cell.y-room.center.y)>=room.half_height) continue;
        int workers=0;
        for (const Entity& actor:game.entities)
            if (actor.entity_a==shift.foreman && (mine_worker(actor.kind) || actor.kind==EntityKind::Strikebreaker)) ++workers;
        return workers;
    }
    const Cell origin=room.center+Cell{-2,-3};
    const Cell locations[]{origin,origin+Cell{1,-1},origin+Cell{1,0},origin+Cell{1,1}};
    int available=0;
    for (const Entity& entity:game.entities) if (entity.kind==EntityKind::None) ++available;
    if (available<4) return 0;
    for (Cell cell:locations) {
        const Tile* tile=game.stage.at(cell);
        if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || entity_at(game,cell,false)>=0 ||
            distance(cell,game.run.spawn)<5) return 0;
    }
    const Handle leader_handle=spawn_entity(game,EntityKind::ShiftForeman,origin);
    Entity& leader=*get_entity(game,leader_handle);
    leader.entity_a=leader_handle; leader.facing={1,0};
    for (int i=0;i<3;++i) {
        Entity& worker=*get_entity(game,spawn_entity(game,EntityKind::Pickhand,locations[i+1]));
        worker.entity_a=leader_handle; worker.counter_b=i-1;
        worker.facing={1,0}; worker.move_wait=i*3;
    }
    // Real supplies placed behind the crew; neither these nor incidental rubble
    // are required to clear the key/switch route.
    place_ground_item(game,room.center+Cell{-5,-3},ItemKind::Pickaxe);
    place_prop(game.stage,room.center+Cell{-5,3},PropKind::Crate);
    if (random_u32(game)%3==0) place_ground_item(game,room.center+Cell{-5,2},ItemKind::ForemanWhistle);
    if (random_u32(game)%3==0) place_ground_item(game,room.center+Cell{-5,1},ItemKind::QuarryCharge,2);
    if (random_u32(game)%3==0) place_ground_item(game,room.center+Cell{-6,1},ItemKind::FuseScissors);
    const auto tool_roll=random_u32(game)%4;
    if (tool_roll<2) place_ground_item(game,room.center+Cell{-6,2},
        tool_roll==0 ? ItemKind::PressHammer : ItemKind::RubberMallet);
    const Cell guard_cell=origin+Cell{-2,1};
    const auto* tile=game.stage.at(guard_cell);
    if ((game.run.floor-1)%4>=1 && available>=5 && tile && walkable(*tile) &&
        tile->kind!=TileKind::Lava && entity_at(game,guard_cell,false)<0) {
        const auto handle=spawn_entity(game,EntityKind::Strikebreaker,guard_cell);
        if (auto* guard=get_entity(game,handle)) {
            guard->entity_a=leader_handle; guard->facing={1,0};
            return 5;
        }
    }
    return 4;
}
