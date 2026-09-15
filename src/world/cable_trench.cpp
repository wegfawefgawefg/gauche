#include "cable_trench.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>
#include <vector>

bool populate_cable_trench(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<2) return false;
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](int x,int y){return room.center+Cell{x*sign,y};};
        std::vector<Cell> wire,wall;
        for (int x=2;x<=6;++x) {wire.push_back(at(x,-3));wall.push_back(at(x,-5));}
        const Cell crawler=at(6,-3),tool=at(-3,2),bin=at(-3,-3),pool=at(4,-2);
        std::vector<Cell> footprint=wire;footprint.insert(footprint.end(),wall.begin(),wall.end());
        for (Cell cell:{tool,bin,pool}) footprint.push_back(cell);
        bool okay=true;
        for (Cell cell:footprint) {
            const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                plan.protected_cell(cell) || distance(cell,game.run.spawn)<6 || entity_at(game,cell,false)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height) {okay=false;break;}
        }
        if (!okay) continue;
        Entity* actor=get_entity(game,spawn_entity(game,EntityKind::CableCrawler,crawler));
        if (!actor) return false;
        actor->facing={-sign,0};
        for (Cell cell:wall) *game.stage.at(cell)={TileKind::Wall,0,0};
        for (Cell cell:wire) place_prop(game.stage,cell,PropKind::CopperWire);
        *game.stage.at(pool)={TileKind::ShallowWater,0,0};
        place_prop(game.stage,bin,PropKind::ScrapBin);
        place_ground_item(game,tool,ItemKind::GroundingSpike);
        return true;
    }
    return false;
}
