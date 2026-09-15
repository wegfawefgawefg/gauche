#include "cooling_works.hpp"
#include "ground_items.hpp"
#include "../surfaces/interaction.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>

bool populate_cooling_works(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<3) return false;
    // The central cross (offset -1..1) is reserved. Keep the vessel and
    // its four-cell vent lane in the side area. Dry routes remain around both
    // ends; the can offers a quick crossing, with a slippery/conductive cost.
    constexpr std::array offsets{Cell{6,-3},Cell{5,-4},Cell{-3,2},Cell{-4,-3},
        Cell{2,-3},Cell{3,-3},Cell{4,-3},Cell{5,-3},Cell{4,-4},Cell{4,-2},Cell{5,-2}};
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
        Entity* tank=get_entity(game,spawn_entity(game,EntityKind::BoilerTank,at({6,-3})));
        if (!tank) return false;
        tank->facing={-sign,0};tank->counter_a=45;tank->counter_b=1800;
        spawn_entity(game,EntityKind::PressureRat,at({5,-4}));
        place_ground_item(game,at({-3,2}),random_u32(game)%2==0 ? ItemKind::CoolantCan : ItemKind::PocketPump);
        for (Cell offset:{Cell{4,-4},Cell{4,-3},Cell{4,-2}}) *game.stage.at(at(offset))={TileKind::Lava,0,0};
        pour_surface(game,at({5,-2}),LiquidKind::Coolant,600);
        place_prop(game.stage,at({-4,-3}),PropKind::OreBin);
        return true;
    }
    return false;
}
