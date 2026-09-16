#include "ice_arches.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>

namespace {
bool natural(const Tile& tile) {
    return tile.kind==TileKind::Ice || tile.kind==TileKind::Snow || tile.kind==TileKind::Empty;
}
bool suitable(const Game& game,const FloorPlan& plan,const RoofSpan& roof) {
    if (roof.kind!=RoofKind::IceArch || !valid_roof(game.stage,roof)) return false;
    const Cell center=roof_cell(roof,roof.length/2,1);
    for (const auto& other:game.stage.roofs)
        if (distance(center,roof_cell(other,other.length/2,1))<7) return false;
    for (int a=0;a<roof.length;++a) for (int b=0;b<3;++b) {
        const Cell cell=roof_cell(roof,a,b);
        const auto& tile=game.stage.at_or_border(cell);
        if (!natural(tile) || surface_wet(tile) || tile.surface.fire_ticks ||
            tile.prop.kind!=PropKind::None || tile.contents!=ItemKind::None) return false;
        const bool foot=b==1 && (a==0 || a==roof.length-1);
        if (foot && plan.protected_cell(cell)) return false;
        for (const auto& actor:game.entities) {
            if (actor.kind==EntityKind::None) continue;
            if (foot && actor.cell==cell) return false;
            if ((actor.kind==EntityKind::Player || actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch ||
                actor.kind==EntityKind::Door || actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate) &&
                distance(actor.cell,cell)<4) return false;
        }
    }
    return true;
}
}

bool place_ice_arch(Game& game,const FloorPlan& plan,RoofSpan roof) {
    if (game.stage.roofs.size()>=max_roof_spans || !suitable(game,plan,roof)) return false;
    const Cell first=roof_cell(roof,0,1),last=roof_cell(roof,roof.length-1,1);
    place_prop(game.stage,first,PropKind::IceArchFoot);
    place_prop(game.stage,last,PropKind::IceArchFoot);
    if (!floor_reachable(game) || !floor_lock_required(game)) {
        game.stage.at(first)->prop={};game.stage.at(last)->prop={};return false;
    }
    game.stage.roofs.push_back(roof);return true;
}

void place_ice_arches(Game& game,const FloorPlan& plan) {
    if (!ice_floor(game.run.floor) || game.run.layout!=FloorLayout::Generated || plan.rooms.empty()) return;
    if (game.run.floor==5 && random_u32(game)%3!=0) return;
    const int desired=game.run.floor==5 ? 1 : 2+static_cast<int>(random_u32(game)%3);
    std::vector<std::size_t> rooms;
    for (std::size_t i=0;i<plan.rooms.size();++i) {
        const auto role=plan.rooms[i].role;
        if (role==RoomRole::IceQuarry || role==RoomRole::Reservoir || role==RoomRole::CliffPath ||
            role==RoomRole::EchoTunnel || role==RoomRole::CrystalGallery || role==RoomRole::WeatherStation) rooms.push_back(i);
    }
    for (std::size_t i=rooms.size();i>1;--i) std::swap(rooms[i-1],rooms[random_u32(game)%i]);
    int count=0;
    for (auto index:rooms) {
        if (count==desired) break;
        if (random_u32(game)%4==0) continue;
        const auto& room=plan.rooms[index];
        std::vector<Cell> sites;
        for (int y=-room.half_height+1;y<room.half_height;++y)
            for (int x=-room.half_width+1;x<room.half_width;++x) {
                const Cell cell=room.center+Cell{x,y};
                if (natural(game.stage.at_or_border(cell))) sites.push_back(cell);
            }
        for (std::size_t i=sites.size();i>1;--i) std::swap(sites[i-1],sites[random_u32(game)%i]);
        for (Cell center:sites) {
            RoofSpan roof;roof.kind=RoofKind::IceArch;roof.hp=static_cast<std::uint8_t>(roof_health(roof.kind));
            roof.height=static_cast<std::uint8_t>(2+random_u32(game)%3);
            roof.length=static_cast<std::uint8_t>(3+random_u32(game)%4);
            roof.vertical=static_cast<std::uint8_t>(random_u32(game)%2);
            roof.start=center-(roof.vertical ? Cell{1,roof.length/2} : Cell{roof.length/2,1});
            if (place_ice_arch(game,plan,roof)) {++count;break;}
        }
    }
}
