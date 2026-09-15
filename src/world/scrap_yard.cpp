#include "scrap_yard.hpp"
#include "ground_items.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <array>

Handle populate_scrap_yard(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<3) return {};
    // Optional hot sorting lane, accessible dry perimeter and a magnet beyond
    // the crane's initial reach. Offsets -1..1 are the reserved central cross;
    // the entire footprint, including pickups and bins, stays outside it.
    constexpr std::array offsets{Cell{6,-3},Cell{2,-3},Cell{-3,2},Cell{-2,-4},Cell{-2,2},
                                Cell{3,-3},Cell{4,-3},Cell{5,-3},Cell{4,-2}};
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
        const Handle crane=spawn_entity(game,EntityKind::MagnetCrane,at({6,-3}));
        if (!get_entity(game,crane)) return {};
        place_ground_item(game,at({2,-3}),ItemKind::Pickaxe);
        for (auto& e:game.entities) if (e.kind==EntityKind::GroundItem && e.cell==at({2,-3})) e.ground_item.durability=6;
        place_ground_item(game,at({-3,2}),ItemKind::HorseshoeMagnet);
        for (Cell offset:{Cell{4,-3},Cell{4,-2}}) *game.stage.at(at(offset))={TileKind::Lava,0,0};
        place_prop(game.stage,at({-2,-4}),PropKind::ScrapBin);
        place_prop(game.stage,at({-2,2}),PropKind::OreBin);
        return crane;
    }
    return {};
}
