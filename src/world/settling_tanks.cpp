#include "settling_tanks.hpp"
#include "ground_items.hpp"
#include "../entities/tar_choir.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

bool populate_settling_tanks(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (std::count_if(game.entities.begin(),game.entities.end(),[](const Entity& e){return e.kind==EntityKind::None;})<5) return false;
    // Dry central crossing remains untouched. Shallow side basins hold actual tar.
    constexpr Cell bodies[]{{-6,-4},{-4,-4},{-2,-4}};
    constexpr Cell tar[]{{-5,2},{-4,2},{-5,3},{-4,3},{4,2},{5,2},{4,3},{5,3}};
    constexpr Cell grates[]{{-6,2},{-6,3},{6,2},{6,3}};
    constexpr Cell cover[]{{-2,-2},{2,-2}};
    constexpr Cell gear[]{{-4,-2},{4,-2},{-5,-2}};
    for (int sign:{room.mirrored ? -1 : 1,room.mirrored ? 1 : -1}) {
        const auto at=[&](Cell offset){return room.center+Cell{offset.x*sign,offset.y};};
        bool okay=true;
        const auto check=[&](Cell offset) {
            const Cell cell=at(offset);const Tile* tile=game.stage.at(cell);
            if (!tile || !walkable(*tile) || tile->kind==TileKind::Lava || tile->prop.kind!=PropKind::None ||
                plan.protected_cell(cell) || distance(cell,game.run.spawn)<6 || entity_at(game,cell,false)>=0 ||
                std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height) okay=false;
        };
        for (Cell c:bodies) check(c);
        for (Cell c:tar) check(c);
        for (Cell c:grates) check(c);
        for (Cell c:cover) check(c);
        for (Cell c:gear) check(c);
        if (!okay) continue;
        if (!get_entity(game,spawn_tar_choir(game,{at(bodies[0]),at(bodies[1]),at(bodies[2])}))) continue;
        for (Cell c:tar) pour_surface(game,at(c),LiquidKind::Tar,1800);
        for (Cell c:grates) place_prop(game.stage,at(c),PropKind::Grate,1);
        for (Cell c:cover) place_prop(game.stage,at(c),PropKind::ScrapBin);
        game.stage.at(at(gear[2]))->kind=TileKind::ShallowWater;
        place_ground_item(game,at(gear[0]),ItemKind::ColdFlask,2);
        place_ground_item(game,at(gear[1]),ItemKind::EmergencyFoam,2);
        return true;
    }
    return false;
}
