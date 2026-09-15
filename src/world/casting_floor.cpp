#include "casting_floor.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

bool populate_casting_floor(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return false;
    constexpr Cell offsets[]{{-4,-3},{-3,-3},{-6,-3},{5,-3},{3,3},{-6,-4},{-6,-2},{5,-4},{5,-2},{-5,-3},{4,-3}};
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
        const Handle thief=spawn_entity(game,EntityKind::MoldThief,at({-4,-3}));
        const Handle mold=spawn_entity(game,EntityKind::CastingMold,at({-6,-3}));
        spawn_entity(game,EntityKind::CastingMold,at({5,-3}));
        if (Entity* actor=get_entity(game,thief)) actor->entity_b=mold;
        Entity* drill=get_entity(game,spawn_entity(game,EntityKind::GroundItem,at({-3,-3})));
        if (drill) {drill->ground_item=make_item(ItemKind::PocketDrill);drill->ground_item.uses=10;drill->sprite=Sprite::PocketDrill;}
        place_ground_item(game,at({3,3}),ItemKind::MoldKey);
        for (Cell offset:{Cell{-6,-4},Cell{-6,-2},Cell{5,-4},Cell{5,-2}}) place_prop(game.stage,at(offset),PropKind::Grate,1);
        return true;
    }
    return false;
}
