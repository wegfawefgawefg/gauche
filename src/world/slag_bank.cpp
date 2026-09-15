#include "slag_bank.hpp"
#include "ground_items.hpp"
#include "../surfaces/slag.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>

bool populate_slag_bank(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<2) return false;
    constexpr std::array offsets{Cell{5,-3},Cell{-3,2},Cell{6,-4},Cell{3,-4},Cell{3,-3},Cell{3,-2},
        Cell{4,-4},Cell{4,-3},Cell{4,-2},Cell{5,-4},Cell{5,-2},Cell{6,-3},Cell{6,-2}};
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
        for (Cell offset:{Cell{3,-4},Cell{3,-3},Cell{3,-2}}) *game.stage.at(at(offset))={TileKind::Lava,0,0};
        leave_slag(game,at({4,-3}));
        spawn_entity(game,EntityKind::SlagSnail,at({5,-3}));
        place_prop(game.stage,at({6,-4}),PropKind::OreBin);
        place_ground_item(game,at({-3,2}),ItemKind::CoolantCan);
        return true;
    }
    return false;
}
