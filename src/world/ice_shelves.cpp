#include "ice_shelves.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../items/supply.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
bool natural_room(const RoomPlan& room) {
    switch (room.role) {
    case RoomRole::IceQuarry: case RoomRole::EchoTunnel: case RoomRole::CliffPath:
    case RoomRole::WeatherStation: case RoomRole::MemorialCourt: case RoomRole::Cache: return true;
    default: return false;
    }
}
void reserve(FloorPlan& plan,Cell cell) {
    plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
}
}

void plan_ice_shelves(Game& game,FloorPlan& plan) {
    if (!ice_floor(game.run.floor) || random_u32(game)%2!=0) return;
    // Keep authored wet/interior rooms intact. A shelf floor needs at least one
    // neighboring natural pair so the profile can open former corridor walls.
    bool pair=false;
    for (const RouteEdge edge:plan.edges)
        if (natural_room(plan.rooms[static_cast<std::size_t>(edge.a)]) &&
            natural_room(plan.rooms[static_cast<std::size_t>(edge.b)])) pair=true;
    if (!pair) return;
    for (RoomPlan& room:plan.rooms) if (natural_room(room)) {
        room.shape=RoomShape::IceShelf;
        room.shelf_variant=static_cast<int>(random_u32(game)%6);
        room.half_width=room.half_height=10;
    }
}

TileKind ice_shelf_floor(const RoomPlan& room,int x,int y) {
    if (room.shelf_variant>=3) std::swap(x,y);
    if (room.mirrored) x=-x;
    const int ax=std::abs(x),ay=std::abs(y);
    if (ax<=1 || ay<=1) return TileKind::Snow;
    // A broad upper/lower shelf, a bent main fissure and a branch reaching the
    // far rim. Safe crossings stay snow-covered; ice occupies the wider lobes.
    const int variation=room.shelf_variant%3;
    const int rim_x=room.half_width-1-((ay+variation)/3)%2;
    const int rim_y=room.half_height-1-((ax+variation)/4)%2;
    const int seam=variation==0 ? 3+x/4 : variation==1 ? -5+ax/3 : 5-x/5;
    const bool branch=variation==0 ? y<-2 && std::abs(x+5-(-y/4))<=1 :
        variation==1 ? y>3 && std::abs(x-5+y/4)<=1 : y<-5 && x<-4;
    if (ax>rim_x || ay>rim_y || std::abs(y-seam)<=1 || branch) return TileKind::Chasm;
    return ax>=rim_x-1 || ay>=rim_y-1 || std::abs(y-seam)==2 ? TileKind::Snow : TileKind::Ice;
}

void connect_ice_shelves(Game& game,FloorPlan& plan) {
    for (RouteEdge edge:plan.edges) {
        const RoomPlan& a=plan.rooms[static_cast<std::size_t>(edge.a)];
        const RoomPlan& b=plan.rooms[static_cast<std::size_t>(edge.b)];
        if (a.shape!=RoomShape::IceShelf || b.shape!=RoomShape::IceShelf) continue;
        const bool horizontal=a.grid.x!=b.grid.x;
        const int middle=horizontal ? (a.center.x+b.center.x)/2 : (a.center.y+b.center.y)/2;
        const Cell turn_a=horizontal ? Cell{middle,a.center.y} : Cell{a.center.x,middle};
        const Cell turn_b=horizontal ? Cell{middle,b.center.y} : Cell{b.center.x,middle};
        const auto segment=[&](Cell from,Cell to) {
            if (from==to) return;
            const Cell along{from.x==to.x ? 0 : to.x>from.x ? 1 : -1,
                             from.y==to.y ? 0 : to.y>from.y ? 1 : -1};
            const Cell across{-along.y,along.x};
            for (;;) {
                if (distance(from,a.center)>=6 && distance(from,b.center)>=6)
                    for (int side=-4;side<=4;++side) {
                        const Cell cell=from+Cell{across.x*side,across.y*side};
                        Tile* tile=game.stage.at(cell);
                        if (!tile || plan.protected_cell(cell)) continue;
                        *tile={std::abs(side)<=2 ? TileKind::Snow : TileKind::Chasm};
                        if (std::abs(side)<=2) reserve(plan,cell);
                    }
                if (from==to) break;
                from=from+along;
            }
        };
        segment(a.center,turn_a);segment(turn_a,turn_b);segment(turn_b,b.center);
        ++plan.shelf_links;
    }
}

void carve_shelf_reward(Game& game,FloorPlan& plan) {
    if (plan.shelf_links==0) return;
    std::vector<std::size_t> choices;
    for (std::size_t i=0;i<plan.rooms.size();++i)
        if (plan.rooms[i].shape==RoomShape::IceShelf) choices.push_back(i);
    for (std::size_t i=choices.size();i>1;--i) std::swap(choices[i-1],choices[random_u32(game)%i]);
    for (auto index:choices) for (int sy:{1,-1}) for (int sx:{1,-1}) {
        const RoomPlan& room=plan.rooms[index];
        const Cell island=room.center+Cell{sx*6,sy*6};
        const Cell bank=room.center+Cell{sx,sy*6};
        bool okay=true;
        for (int y=-4;y<=4 && okay;++y) for (int x=-4;x<=4;++x) {
            const Cell cell=island+Cell{x,y};const Tile* tile=game.stage.at(cell);
            if (!tile || tile->kind==TileKind::Wall || tile->break_rule==BreakRule::Unbreakable || plan.protected_cell(cell) ||
                tile->prop.kind!=PropKind::None || entity_at(game,cell,false)>=0) {okay=false;break;}
        }
        if (!okay || !walkable(game.stage.at_or_border(bank)) ||
            !walkable(game.stage.at_or_border(bank-Cell{sx,0}))) continue;
        // Nine cells of real landing space, surrounded by a three-cell gap.
        // Reserve landing/approach from later clutter; the void itself stays void.
        for (int y=-4;y<=4;++y) for (int x=-4;x<=4;++x) {
            const Cell cell=island+Cell{x,y};const bool land=std::abs(x)<=1 && std::abs(y)<=1;
            *game.stage.at(cell)={land ? TileKind::Snow : TileKind::Chasm};
            if (land) reserve(plan,cell);
        }
        *game.stage.at(bank)={TileKind::Snow};reserve(plan,bank);
        reserve(plan,bank-Cell{sx,0});
        plan.shelf_rewards.push_back({island,bank,{sx,0}});
        return; // One optional extra reward, not a second required objective.
    }
}

void populate_shelf_reward(Game& game,const FloorPlan& plan) {
    for (const auto& reward:plan.shelf_rewards) {
        const ItemKind weapon=roll_item_supply(game,LootSource::Weapon,false);
        if (weapon==ItemKind::None) continue;
        place_ground_item(game,reward.island,weapon,supply_count(weapon));
        place_coins(game,reward.island+Cell{0,1},12+static_cast<int>(random_u32(game)%7));
        // This permanent crossing also provides a way back. Other carried tools
        // can recover the prize, but the level never requires a lucky gear drop.
        place_ground_item(game,reward.bank-reward.direction,ItemKind::FoldedBridge);
    }
}
