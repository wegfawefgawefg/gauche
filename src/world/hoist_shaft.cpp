#include "hoist_shaft.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>

bool populate_hoist_shaft(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return false;
    constexpr std::array offsets{Cell{-5,-4},Cell{-3,-4},Cell{-5,-2},Cell{-2,-2},Cell{-2,-3},Cell{3,3},Cell{-2,3}};
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
        spawn_entity(game,EntityKind::Counterweight,at({-5,-4}));
        spawn_entity(game,EntityKind::PressureRat,at({-2,-2}));
        place_prop(game.stage,at({-3,-4}),PropKind::ScrapBin);
        place_prop(game.stage,at({-5,-2}),PropKind::OreBin);
        place_coins(game,at({-2,-3}),8);
        place_ground_item(game,at({3,3}),ItemKind::CoolantCan);
        place_ground_item(game,at({-2,3}),ItemKind::ChainHook);
        return true;
    }
    return false;
}
