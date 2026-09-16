#include "ice_thaw.hpp"
#include "stream_work.hpp"
#include "ground_items.hpp"
#include "loot.hpp"
#include "../items/supply.hpp"
#include "water.hpp"
#include "../props/interaction.hpp"
#include <algorithm>
#include <cstdlib>

namespace {
bool natural(const RoomPlan& room) {
    switch (room.role) {
    case RoomRole::Reservoir: case RoomRole::FishingHut: case RoomRole::IceQuarry:
    case RoomRole::EchoTunnel: case RoomRole::CliffPath: case RoomRole::Cache: return true;
    default: return false;
    }
}
Cell scale(Cell cell,int amount) {return {cell.x*amount,cell.y*amount};}
void reserve(FloorPlan& plan,Cell cell) {
    plan.protected_cells[static_cast<std::size_t>(cell.y*plan.width+cell.x)]=1;
}
void paint(Game& game,FloorPlan& plan,Cell cell,TileKind kind) {
    if (Tile* tile=game.stage.at(cell)) {*tile={kind};reserve(plan,cell);}
}
void line(std::vector<Cell>& cells,Cell from,Cell to) {
    while (from!=to) {cells.push_back(from);from=from+cardinal_toward(from,to,{1,0});}
    cells.push_back(to);
}
}

void plan_ice_thaw(Game& game,FloorPlan& plan) {
    if (!ice_floor(game.run.floor)) return;
    for (const auto& room:plan.rooms) if (room.shape==RoomShape::IceShelf) return;
    std::vector<ThawChannel> choices;
    for (auto edge:plan.edges) {
        const auto& a=plan.rooms[static_cast<std::size_t>(edge.a)];
        const auto& b=plan.rooms[static_cast<std::size_t>(edge.b)];
        if (!natural(a) || !natural(b)) continue;
        const Cell along=cardinal_toward(a.center,b.center,{1,0});
        for (int sign:{-1,1}) {
            const Cell across=scale({-along.y,along.x},sign);
            bool clear=true;
            // A required doorway carves a dry approach to the room center.
            // Keep the river on the opposite shore so that later gate carving
            // cannot dam the channel or erase its salvage island.
            for (auto link:plan.edges) for (int index:{edge.a,edge.b}) {
                const int neighbor=link.a==index ? link.b : link.b==index ? link.a : -1;
                if (neighbor<0 || (neighbor!=plan.exit_room && neighbor!=plan.secret_room)) continue;
                const Cell side=plan.rooms[static_cast<std::size_t>(neighbor)].grid-
                    plan.rooms[static_cast<std::size_t>(index)].grid;
                if (side.x*across.x+side.y*across.y>0) clear=false;
            }
            if (clear) {ThawChannel channel;channel.a=edge.a;channel.b=edge.b;
                channel.along=along;channel.across=across;choices.push_back(channel);}
        }
    }
    if (choices.empty()) return;
    const auto channel=choices[random_u32(game)%choices.size()];
    for (int index:{channel.a,channel.b}) {
        auto& room=plan.rooms[static_cast<std::size_t>(index)];
        room.shape=RoomShape::ThawCavern;room.half_width=room.half_height=10;
    }
    plan.thaw_channels.push_back(channel);
}

TileKind ice_thaw_floor(const RoomPlan& room,int x,int y) {
    // Broad cavern with a dry ring around the meltwater's ends. The second
    // carving pass opens the wall between paired rooms, not just their corridor.
    if (std::abs(x)>=8 || std::abs(y)>=8 || std::abs(x)<=1 || std::abs(y)<=1) return TileKind::Snow;
    return (x+(room.mirrored ? y : -y))%5==0 ? TileKind::Ice : TileKind::Snow;
}

void carve_ice_thaw(Game& game,FloorPlan& plan) {
    for (auto& channel:plan.thaw_channels) {
        const Cell a=plan.rooms[static_cast<std::size_t>(channel.a)].center;
        const Cell b=plan.rooms[static_cast<std::size_t>(channel.b)].center;
        const Cell along=channel.along,across=channel.across;
        // The shared cavern has broad dry shores. The outer dry ring remains
        // intact in both rooms, so every incoming corridor can go around water.
        const Cell turn=along.x ? Cell{b.x,a.y} : Cell{a.x,b.y};
        std::vector<Cell> spine;line(spine,a,turn);line(spine,turn,b);
        for (Cell center:spine) for (int width=-8;width<=8;++width)
            paint(game,plan,center+scale(across,width),TileKind::Snow);
        std::vector<Cell> stream;
        channel.source=a-scale(along,6)+scale(across,5);
        channel.mouth=b+scale(along,6)+scale(across,5);
        const Cell bend=a+scale(along,8)+scale(across,4);
        const Cell turn_water=along.x ? Cell{b.x,bend.y} : Cell{bend.x,b.y};
        line(stream,channel.source,bend);line(stream,bend,turn_water);line(stream,turn_water,channel.mouth);
        // Three-cell watercourse: wet shallows edge the deep center. Widen its
        // bends with cardinal neighbors so swimming and electrical paths connect.
        for (Cell center:stream) for (Cell side:{Cell{},Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}})
            paint(game,plan,center+side,TileKind::ShallowWater);
        for (Cell center:stream) paint(game,plan,center,TileKind::Water);
        paint(game,plan,channel.source,TileKind::Spring);
        channel.prize=b+scale(along,4)+scale(across,5);
        channel.bank=channel.prize-scale(across,4);
        // A thaw pool encloses one dry salvage plinth. The three-cell cast/span
        // gives the line and bridge concrete jobs; required objectives stay out.
        for (int y=-3;y<=3;++y) for (int x=-3;x<=3;++x)
            paint(game,plan,channel.prize+Cell{x,y},std::abs(x)==3 || std::abs(y)==3 ? TileKind::ShallowWater : TileKind::Water);
        paint(game,plan,channel.prize,TileKind::Snow);
        paint(game,plan,channel.bank,TileKind::Snow);
        paint(game,plan,channel.bank-across,TileKind::Snow);
        channel.frozen=a-scale(across,5)-scale(along,4);
        for (int y=-2;y<=2;++y) for (int x=-2;x<=2;++x)
            paint(game,plan,channel.frozen+Cell{x,y},TileKind::Ice);
        paint(game,plan,channel.frozen,TileKind::IceHole);
        // Currents follow the actual watercourse. Deep water doesn't advect land
        // actors; its shallow margins carry cargo downstream into the wider pool.
        for (std::size_t i=0;i<stream.size();++i) {
            const Cell toward=i+1<stream.size() ? stream[i+1]-stream[i] : along;
            if (distance({},toward)!=1) continue;
            const auto code=static_cast<std::uint8_t>(toward.x>0 ? 1 : toward.y>0 ? 2 : toward.x<0 ? 3 : 4);
            for (Cell side:{Cell{},Cell{1,0},Cell{-1,0},Cell{0,1},Cell{0,-1}}) {
                Tile* tile=game.stage.at(stream[i]+side);
                if (tile && shallow_water(tile->kind) && tile->kind!=TileKind::IceHole) tile->current=code;
            }
        }
        channel.eel=stream[stream.size()/2];
    }
}

void populate_ice_thaw(Game& game,const FloorPlan& plan) {
    for (const auto& channel:plan.thaw_channels) {
        const ItemKind reward=roll_item_supply(game,LootSource::Salvage,false);
        place_coins(game,channel.prize,10+static_cast<int>(random_u32(game)%9));
        place_ground_item(game,channel.prize,reward,supply_count(reward));
        place_ground_item(game,channel.bank-channel.across,ItemKind::FishingLine);
        place_ground_item(game,channel.bank-channel.along,ItemKind::FoldedBridge);
        for (int index:{channel.a,channel.b}) {
            const Cell center=plan.rooms[static_cast<std::size_t>(index)].center;
            for (int offset:{-6,0,6}) {
                const Cell cell=center-scale(channel.across,7)+scale(channel.along,offset);
                const Tile& tile=game.stage.at_or_border(cell);
                if (tile.kind==TileKind::Snow && entity_at(game,cell,false)<0)
                    place_prop(game.stage,cell,offset==0 ? PropKind::FishingCreel : PropKind::SnowCache);
            }
        }
        populate_stream_work(game,channel);
        // A small guaranteed habitat population; ordinary room encounters skip
        // these two rooms rather than duplicating their aquatic specialists.
        spawn_entity(game,EntityKind::GlassEel,channel.eel);
        spawn_entity(game,EntityKind::BellDiver,channel.frozen);
        if ((game.run.floor-1)%4>=2)
            spawn_entity(game,EntityKind::RimeSkater,channel.frozen+channel.along);
    }
}
