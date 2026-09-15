#include "freight_siding.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include <algorithm>

bool populate_freight_siding(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<3) return false;
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](int x,int y){return room.center+Cell{x*sign,y};};
        bool okay=true;
        // The track crosses the reserved path only as ordinary walkable floor.
        // Bodies and supplies start wholly outside that crossing.
        for (int x=-7;x<=7;++x) {
            const Cell cell=at(x,-3);const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                entity_at(game,cell,false)>=0 || distance(cell,game.run.spawn)<6 ||
                std::abs(cell.x-room.center.x)>=room.half_width) {okay=false;break;}
        }
        for (Cell cell:{at(-6,-3),at(-5,-3),at(-4,2)}) {
            const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                entity_at(game,cell,false)>=0 || plan.protected_cell(cell)) okay=false;
        }
        if (!okay) continue;
        // A branch is optional, but must fit completely before any tiles change.
        bool branch=!plan.protected_cell(at(3,-3));
        for (int y=-6;y<=-3;++y) for (int x=2;x<=4;++x) {
            const Cell cell=at(x,y);const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                entity_at(game,cell,false)>=0 || std::abs(y)>=room.half_height) branch=false;
        }
        for (int x=-6;x<=6;++x) *game.stage.at(at(x,-3))={TileKind::Rail,24,0,24,BreakRule::DigRequired,1};
        if (branch) {
            for (int y=-5;y<=-4;++y) *game.stage.at(at(3,y))={TileKind::Rail,24,0,24,BreakRule::DigRequired,1};
            place_prop(game.stage,at(3,-3),PropKind::RailPoints,sign>0 ? 0 : 2);
        }
        const Handle worker=spawn_entity(game,EntityKind::RailShunter,at(-6,-3));
        const Handle cart=spawn_entity(game,EntityKind::RailCart,at(-5,-3));
        get_entity(game,worker)->entity_a=cart;get_entity(game,cart)->facing={sign,0};
        Item load=make_item(random_u32(game)%2==0 ? ItemKind::BoltPouch : ItemKind::PressHammer);
        if (load.kind==ItemKind::BoltPouch) load.count=3;
        else load.durability=12;
        get_entity(game,cart)->ground_item=load;
        place_ground_item(game,at(-4,2),branch ? ItemKind::RailSwitchKey : ItemKind::BrakeShoe);
        return true;
    }
    return false;
}
