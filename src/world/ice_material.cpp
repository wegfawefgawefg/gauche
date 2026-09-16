#include "ice_material.hpp"
#include "ground_items.hpp"
#include "ice_render.hpp"
#include <algorithm>

namespace {
bool dry(const Tile& tile) {
    return walkable(tile) && (tile.kind==TileKind::Snow || tile.kind==TileKind::Empty ||
        tile.kind==TileKind::Ruin || tile.kind==TileKind::Ice);
}
bool masonry(const FloorPlan& plan,Cell cell) {
    for (std::size_t i=0;i<plan.rooms.size();++i) {
        const auto& room=plan.rooms[i];
        const bool built=static_cast<int>(i)==plan.exit_room || static_cast<int>(i)==plan.secret_room ||
            room.role==RoomRole::Chapel || room.role==RoomRole::Shelter || room.role==RoomRole::Observatory ||
            room.role==RoomRole::ServicePassage || room.role==RoomRole::BoilerGallery;
        if (built && std::abs(cell.x-room.center.x)<=room.half_width+1 &&
            std::abs(cell.y-room.center.y)<=room.half_height+1) return true;
    }
    return false;
}
}

void place_ice_materials(Game& game,const FloorPlan& plan) {
    // Coarse strata run through neighboring cells instead of random tile noise.
    // Buildings and required gate surrounds keep a proper rock backing.
    const auto salt=random_u32(game)%17;
    std::vector<Cell> pockets;
    std::vector<bool> reached(game.stage.tiles.size());
    std::vector<Cell> banks{plan.rooms[0].center};
    for (std::size_t next=0;next<banks.size();++next) {
        const Cell cell=banks[next];const Tile* tile=game.stage.at(cell);
        if (!tile || !dry(*tile)) continue;
        const auto index=static_cast<std::size_t>(cell.y*game.stage.width+cell.x);
        if (reached[index]) continue;
        reached[index]=true;
        for (Cell side:{Cell{1,0},{-1,0},{0,1},{0,-1}}) banks.push_back(cell+side);
    }
    for (int y=0;y<game.stage.height;++y) for (int x=0;x<game.stage.width;++x) {
        const Cell cell{x,y};Tile& tile=*game.stage.at(cell);
        if (tile.kind!=TileKind::Wall || tile.material!=TileMaterial::Stone) continue;
        const bool rock=tile.break_rule==BreakRule::Unbreakable || masonry(plan,cell) ||
            ((x/4+y/3+static_cast<int>(salt))%7<2);
        if (rock) continue;
        tile.material=TileMaterial::Ice;
        // Damageable secret gates keep their authored durability. Diggable ice
        // has a clear 60-HP seam or a thicker cloudy 100-HP body.
        if (tile.break_rule==BreakRule::DigRequired) {
            tile.hp=tile.max_hp=static_cast<std::uint16_t>((x/3-y/4+static_cast<int>(salt))%3==0 ? 60 : 100);
        }
        if (tile.max_hp>60 || plan.protected_cell(cell)) continue;
        for (Cell side:{Cell{1,0},{-1,0},{0,1},{0,-1}})
            if (const Cell bank=cell+side;game.stage.in_bounds(bank) &&
                reached[static_cast<std::size_t>(bank.y*game.stage.width+bank.x)]) {pockets.push_back(cell);break;}
    }
    for (std::size_t i=pockets.size();i>1;--i) std::swap(pockets[i-1],pockets[random_u32(game)%i]);
    constexpr ItemKind finds[]{ItemKind::SmokedFish,ItemKind::Bandage,ItemKind::Chisel,
        ItemKind::Crampons,ItemKind::HarpoonGun,ItemKind::HeatCapsule};
    std::vector<Cell> placed;
    for (Cell cell:pockets) {
        bool close=false;for (Cell other:placed) if (distance(cell,other)<12) close=true;
        if (close) continue;
        Tile& tile=*game.stage.at(cell);
        tile.contents=finds[random_u32(game)%std::size(finds)];
        tile.content_count=tile.contents==ItemKind::SmokedFish || tile.contents==ItemKind::Bandage ? 2 : 1;
        placed.push_back(cell);
        if (placed.size()>=2) break;
    }
}

void release_wall_contents(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || tile->kind==TileKind::Wall || tile->contents==ItemKind::None) return;
    const auto kind=tile->contents;const int count=tile->content_count;
    tile->contents=ItemKind::None;tile->content_count=0;
    place_ground_item(game,cell,kind,count);
}

bool melt_ice_wall(Game& game,Cell cell,int damage,Cell source) {
    Tile* tile=game.stage.at(cell);
    if (!tile || tile->kind!=TileKind::Wall || tile->material!=TileMaterial::Ice ||
        tile->break_rule==BreakRule::Unbreakable || damage<=0) return false;
    const Sprite sprite=ice_wall_sprite(*tile,cell);
    const int before=tile->hp;
    if (!damage_tile(game.stage,cell,damage,255)) return false;
    if (game.impact_count<static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)]={cell,source,sprite,before-tile->hp,tile->hp==0};
    if (tile->hp==0) {tile->kind=TileKind::ShallowWater;release_wall_contents(game,cell);emit_sound(game,SoundId::IceThaw,cell);}
    return true;
}
