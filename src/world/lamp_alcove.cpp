#include "lamp_alcove.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

bool populate_lamp_alcove(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<3) return false;
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](int x,int y){return room.center+Cell{x*sign,y};};
        const Cell stove=at(6,-3),lamp=at(3,-5),first=at(5,-3),second=at(3,-4),tool=at(-3,2);
        bool okay=true;
        for (Cell cell:{stove,lamp,first,second,tool,at(-4,2),at(4,-3),at(4,-4),at(5,-4)}) {
            const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                plan.protected_cell(cell) || distance(cell,game.run.spawn)<6 || entity_at(game,cell,false)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height) {okay=false;break;}
        }
        if (!okay) continue;
        place_prop(game.stage,stove,PropKind::Stove);
        game.stage.at(stove)->prop.growth_ticks=1800;game.stage.at(stove)->prop.variant=1;
        place_prop(game.stage,lamp,PropKind::BeamLamp);
        Entity* moth=get_entity(game,spawn_entity(game,EntityKind::FurnaceMoth,first));
        moth->counter_a=0;moth->sprite=Sprite::FurnaceMoth;
        spawn_entity(game,EntityKind::FurnaceMoth,second);
        game.stage.at(at(-4,2))->kind=TileKind::ShallowWater;
        if (random_u32(game)%2==0) {
            Entity* gear=get_entity(game,spawn_entity(game,EntityKind::GroundItem,tool));
            gear->ground_item=make_item(ItemKind::SteamLance);gear->ground_item.loaded=1;gear->sprite=Sprite::SteamLance;
        } else place_ground_item(game,tool,ItemKind::EmergencyFoam,2);
        return true;
    }
    return false;
}
