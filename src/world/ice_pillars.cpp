#include "ice_pillars.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include <algorithm>
#include <cstdlib>
#include <vector>

namespace {
bool outdoor(const RoomPlan& room) {
    return room.role!=RoomRole::Entrance && room.role!=RoomRole::Exit && room.role!=RoomRole::Shrine &&
        room.role!=RoomRole::Chapel && room.role!=RoomRole::Shelter && room.role!=RoomRole::FishingHut &&
        room.role!=RoomRole::Bathhouse && room.role!=RoomRole::Observatory && room.role!=RoomRole::MemorialCourt;
}
bool socket(const Game& game,const FloorPlan& plan,Cell cell) {
    const auto* tile=game.stage.at(cell);
    if (!tile || !walkable(*tile) || surface_wet(*tile) || tile->kind==TileKind::Lava ||
        tile->kind==TileKind::Ruin || tile->prop.kind!=PropKind::None || plan.protected_cell(cell)) return false;
    for (const auto& actor:game.entities) {
        if (actor.kind==EntityKind::None) continue;
        if (actor.cell==cell) return false;
        if ((actor.kind==EntityKind::Player || actor.kind==EntityKind::Key || actor.kind==EntityKind::Switch ||
            actor.kind==EntityKind::Door || actor.kind==EntityKind::Exit || actor.kind==EntityKind::EncounterGate) &&
            distance(actor.cell,cell)<4) return false;
    }
    return true;
}
std::vector<Cell> sockets(const Game& game,const FloorPlan& plan,const RoomPlan& room) {
    std::vector<Cell> cells;
    for (int y=-room.half_height+1;y<room.half_height;++y)
        for (int x=-room.half_width+1;x<room.half_width;++x) {
            const Cell c=room.center+Cell{x,y};if (socket(game,plan,c)) cells.push_back(c);
        }
    return cells;
}
void shuffle(Game& game,std::vector<Cell>& cells) {
    for (std::size_t i=cells.size();i>1;--i) std::swap(cells[i-1],cells[random_u32(game)%i]);
}
void clutter(Game& game,const FloorPlan& plan,const RoomPlan& room) {
    if (room.role==RoomRole::Entrance || room.role==RoomRole::Exit || random_u32(game)%5==0) return;
    auto anchors=sockets(game,plan,room);shuffle(game,anchors);
    const int groups=2+static_cast<int>(anchors.size()/60);
    std::vector<Cell> centers;
    for (Cell anchor:anchors) {
        if (static_cast<int>(centers.size())>=groups) break;
        if (!socket(game,plan,anchor) || std::any_of(centers.begin(),centers.end(),
            [&](Cell c){return distance(c,anchor)<4;})) continue;
        centers.push_back(anchor);
        bool icy=false;
        for (Cell d:{Cell{},Cell{1,0},Cell{0,1},Cell{-1,0},Cell{0,-1}})
            icy|=game.stage.at_or_border(anchor+d).kind==TileKind::Ice;
        const bool sharp=outdoor(room) && icy && random_u32(game)%3!=0;
        const int radius=2+static_cast<int>(random_u32(game)%3);
        const int pieces=5+static_cast<int>(random_u32(game)%9);
        std::vector<Cell> children;
        for (Cell c:anchors)
            if (distance(c,anchor)<=radius && socket(game,plan,c)) children.push_back(c);
        shuffle(game,children);
        int count=0;
        for (Cell c:children) {
            if (count++==pieces) break;
            // Roll occupied child sockets, not repeated attempts at invalid walls.
            // Snow shoulders mix into sharp patches; routes retain their clear lanes.
            const bool spike=sharp && random_u32(game)%4!=0;
            place_prop(game.stage,c,spike ? PropKind::IceSpikes : PropKind::SnowPile,
                static_cast<std::uint8_t>(random_u32(game)%3));
        }
    }
}
}

void place_ice_pillars(Game& game,const FloorPlan& plan) {
    if (!ice_floor(game.run.floor) || plan.rooms.empty()) return;
    // Per-room rolls replace the floor-wide 3–5 cap. Some rooms stay quiet;
    // others grow a small grove, followed by independent ground-level clusters.
    std::vector<Cell> placed;
    for (const auto& room:plan.rooms) {
        if (!outdoor(room) || random_u32(game)%4==0) continue;
        auto choices=sockets(game,plan,room);shuffle(game,choices);
        const int desired=1+static_cast<int>(random_u32(game)%3);
        const int spacing=2+static_cast<int>(random_u32(game)%4);
        int count=0;
        for (Cell cell:choices) {
            if (count==desired) break;
            if (std::any_of(placed.begin(),placed.end(),[&](Cell old){return distance(cell,old)<spacing;})) continue;
            int open=0;
            for (Cell d:{Cell{1,0},Cell{0,1},Cell{-1,0},Cell{0,-1}})
                open+=walkable(game.stage.at_or_border(cell+d));
            if (open<3) continue;
            const auto variant=static_cast<std::uint8_t>((random_u32(game)%4)*64+random_u32(game)%4);
            if (!place_prop(game.stage,cell,PropKind::IcePillar,variant)) continue;
            if (!floor_reachable(game)) {game.stage.at(cell)->prop={};continue;}
            placed.push_back(cell);++count;
        }
    }
    for (const auto& room:plan.rooms) clutter(game,plan,room);
}
