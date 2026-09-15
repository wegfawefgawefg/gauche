#include "kiln_court.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <vector>

bool populate_kiln_court(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<2) return false;
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](int x,int y){return room.center+Cell{x*sign,y};};
        const Cell kiln=at(6,-3),tool=at(-3,2),crate=at(6,-4),log=at(5,-5);
        std::vector<Cell> footprint{kiln,tool,crate,log};
        // Keep the sweep court and its dry margins clear of other placed content.
        for (int x=2;x<=5;++x) for (int y=-4;y<=-2;++y) footprint.push_back(at(x,y));
        bool okay=true;
        for (Cell cell:footprint) {
            const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                plan.protected_cell(cell) || distance(cell,game.run.spawn)<6 || entity_at(game,cell,false)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height) {okay=false;break;}
        }
        if (!okay) continue;
        Entity* actor=get_entity(game,spawn_entity(game,EntityKind::WalkingKiln,kiln));
        if (!actor) return false;
        actor->facing={-sign,0};
        place_prop(game.stage,crate,PropKind::Crate);
        place_prop(game.stage,log,PropKind::RottenLog);
        for (int x=3;x<=5;++x) {
            Tile& tile=*game.stage.at(at(x,-3));
            tile.surface.liquid=LiquidKind::Oil;tile.surface.liquid_ticks=1800;
        }
        place_ground_item(game,tool,ItemKind::CoolantCan);
        return true;
    }
    return false;
}
