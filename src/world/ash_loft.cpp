#include "ash_loft.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include "../props/conveyor.hpp"
#include <algorithm>
#include <array>

bool populate_ash_loft(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return false;
    constexpr std::array offsets{Cell{-4,-3},Cell{-2,-3},Cell{-5,-4},Cell{4,-3},Cell{2,3},Cell{-4,3},
        Cell{-5,2},Cell{-5,3},Cell{-5,4}};
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](Cell offset){return room.center+Cell{offset.x*sign,offset.y};};
        bool okay=true;
        for (Cell offset:offsets) {
            const Cell cell=at(offset);const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                plan.protected_cell(cell) || distance(cell,game.run.spawn)<6 || entity_at(game,cell,false)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height) {okay=false;break;}
        }
        if (!okay) continue;
        for (Cell offset:{Cell{-4,-3},Cell{-2,-3}}) spawn_entity(game,EntityKind::AshSleeper,at(offset));
        place_prop(game.stage,at({-5,-4}),PropKind::ScrapBin);
        place_prop(game.stage,at({4,-3}),PropKind::OreBin);
        for (int y=2;y<=4;++y) place_prop(game.stage,at({-5,y}),PropKind::Conveyor,1|belt_manual);
        place_ground_item(game,at({-5,2}),ItemKind::NailBoard);
        place_ground_item(game,at({-4,3}),ItemKind::BeltCrank);
        place_ground_item(game,at({2,3}),ItemKind::CoolantCan);
        return true;
    }
    return false;
}
