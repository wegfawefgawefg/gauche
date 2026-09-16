#include "roof_scenes.hpp"
#include "terrain_material.hpp"
#include "../scenery/roof.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <vector>

namespace {
bool dry_floor(const Tile& tile) {
    return tile.kind==TileKind::Empty || tile.kind==TileKind::Grass || tile.kind==TileKind::Ruin ||
        tile.kind==TileKind::Snow || tile.kind==TileKind::Ice;
}
bool empty_site(const Game& game,const FloorPlan& plan,const RoofSpan& roof,bool shortcut) {
    if (!valid_roof(game.stage,roof)) return false;
    int walls=0;
    for (int along=-1;along<=roof.length;++along) for (int across=0;across<3;++across) {
        const Cell cell=roof_cell(roof,along,across);
        const Tile* tile=game.stage.at(cell);
        if (!tile || cell.x<2 || cell.y<2 || cell.x>=game.stage.width-2 || cell.y>=game.stage.height-2 ||
            tile->prop.kind!=PropKind::None || tile->contents!=ItemKind::None || tile->surface.fire_ticks>0) return false;
        for (const RoofSpan& other:game.stage.roofs)
            if (distance(roof_cell(roof,roof.length/2,1),roof_cell(other,other.length/2,1))<12) return false;
        const bool interior=along>=0 && along<roof.length;
        const bool wall=tile->kind==TileKind::Wall;
        if (wall) {
            if (!shortcut || !interior || tile->break_rule==BreakRule::Unbreakable || plan.protected_cell(cell)) return false;
            if (across==1) ++walls;
        } else if (!dry_floor(*tile)) return false;
        if (interior && across!=1 && plan.protected_cell(cell)) return false;
        for (const Entity& actor:game.entities) {
            if (actor.kind==EntityKind::None) continue;
            if (actor.cell==cell) return false;
            if ((actor.kind==EntityKind::Door || actor.kind==EntityKind::Exit || actor.kind==EntityKind::Key ||
                 actor.kind==EntityKind::Switch || actor.kind==EntityKind::EncounterGate || actor.kind==EntityKind::Encounter ||
                 actor.kind==EntityKind::Player) && distance(actor.cell,cell)<4) return false;
        }
    }
    return !shortcut || (walls>0 && walls<roof.length);
}

bool place(Game& game,const FloorPlan& plan,RoofSpan roof,bool shortcut) {
    if (game.stage.roofs.size()>=max_roof_spans || !empty_site(game,plan,roof,shortcut)) return false;
    std::vector<Tile> before;
    for (int along=0;along<roof.length;++along) for (int across=0;across<3;++across) {
        const Cell cell=roof_cell(roof,along,across);
        Tile& tile=*game.stage.at(cell);before.push_back(tile);
        if (across==1) {
            if (tile.kind==TileKind::Wall) tile={TileKind::Grass};
        } else if (roof.kind==RoofKind::Log) tile=wood_tile(TileMaterial::Timber);
        else if (roof.kind==RoofKind::IceArch) {
            tile={TileKind::Wall,40,0,40,BreakRule::Damageable,0};tile.material=TileMaterial::Ice;
        } else place_prop(game.stage,cell,PropKind::Grate,roof.vertical ? 0 : 1);
    }
    // Run after objective/prop population. A shortcut may bypass ordinary rock,
    // never the required exit lock. Failed edits restore every affected tile.
    if (!floor_reachable(game) || !floor_lock_required(game)) {
        std::size_t index=0;
        for (int along=0;along<roof.length;++along) for (int across=0;across<3;++across)
            *game.stage.at(roof_cell(roof,along,across))=before[index++];
        return false;
    }
    game.stage.roofs.push_back(roof);
    return true;
}
}

void place_roof_scenes(Game& game,const FloorPlan& plan) {
    if (game.run.layout!=FloorLayout::Generated || plan.rooms.empty()) return;
    const RoofKind kind=forest_floor(game.run.floor) ? RoofKind::Log : ice_floor(game.run.floor) ? RoofKind::IceArch : RoofKind::Gantry;
    const int desired=2+static_cast<int>(random_u32(game)%2);
    // Normal scenes get their own small budget, not the special-encounter budget.
    const int offset=static_cast<int>(random_u32(game)%static_cast<unsigned>(plan.rooms.size()));
    for (std::size_t i=0;i<plan.rooms.size() && static_cast<int>(game.stage.roofs.size())<desired;++i) {
        const RoomPlan& room=plan.rooms[(i+static_cast<std::size_t>(offset))%plan.rooms.size()];
        if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit) continue;
        for (int attempt=0;attempt<90;++attempt) {
            RoofSpan roof;roof.kind=kind;roof.hp=static_cast<std::uint8_t>(roof_health(kind));
            roof.length=static_cast<std::uint8_t>((kind==RoofKind::Log ? 5 : 3)+random_u32(game)%3);
            roof.vertical=static_cast<std::uint8_t>(random_u32(game)%2);
            roof.start=room.center+Cell{
                static_cast<int>(random_u32(game)%static_cast<unsigned>(room.half_width*2+1))-room.half_width,
                static_cast<int>(random_u32(game)%static_cast<unsigned>(room.half_height*2+1))-room.half_height};
            if (place(game,plan,roof,false)) break;
        }
    }
    // Rare cross-room hollow timber opening. Ordinary roof scenes above are
    // common; this separate attempt budget is the optional wall-crossing feature.
    if (kind!=RoofKind::Log || random_u32(game)%4!=0) return;
    const int size=game.stage.width*game.stage.height;
    const int start=static_cast<int>(random_u32(game)%static_cast<unsigned>(size));
    for (int i=0;i<size;++i) {
        const int index=(start+i)%size;
        RoofSpan roof;roof.start={index%game.stage.width,index/game.stage.width};roof.length=7;
        roof.vertical=static_cast<std::uint8_t>(i%2);
        if (place(game,plan,roof,true)) break;
    }
}
