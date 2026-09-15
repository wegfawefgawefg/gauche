#include "pay_office.hpp"
#include "loot.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

bool populate_pay_office(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<3) return false;
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](int x,int y){return room.center+Cell{x*sign,y};};
        const Cell clerk=at(3,-3),money=at(4,-3),cage=at(6,-3),worker=at(2,-5);
        bool okay=true;
        for (Cell cell:{clerk,money,cage,worker,at(5,-3),at(6,-4),at(6,-2)}) {
            const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                plan.protected_cell(cell) || distance(cell,game.run.spawn)<6 || entity_at(game,cell,false)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height) {okay=false;break;}
        }
        if (!okay) continue;
        Entity* actor=get_entity(game,spawn_entity(game,EntityKind::AuditClerk,clerk));
        if (!actor) return false;
        actor->point_a=cage;actor->facing={sign,0};
        spawn_entity(game,EntityKind::Pickhand,worker);
        place_prop(game.stage,cage,PropKind::PayCage);
        place_prop(game.stage,at(6,-4),PropKind::Grate,1);
        place_prop(game.stage,at(6,-2),PropKind::Grate,1);
        if (random_u32(game)%100<60)
            game.stage.at(cage)->prop.growth_ticks=static_cast<std::uint16_t>(8+random_u32(game)%7);
        place_coins(game,money,6);
        return true;
    }
    return false;
}
