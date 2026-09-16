#include "chasm.hpp"
#include "route.hpp"
#include <algorithm>
#include <utility>

namespace {
bool fissure_room(RoomRole role) {
    switch (role) {
    case RoomRole::IceQuarry: case RoomRole::EchoTunnel: case RoomRole::CliffPath:
    case RoomRole::ServicePassage: case RoomRole::MemorialCourt: case RoomRole::WeatherStation:
    case RoomRole::Clearing: case RoomRole::Thicket: case RoomRole::Ruins:
    case RoomRole::Cache: case RoomRole::Workshop: case RoomRole::Den: return true;
    default: return false;
    }
}
bool dry(TileKind kind) {
    return kind==TileKind::Empty || kind==TileKind::Grass || kind==TileKind::Ruin ||
        kind==TileKind::Snow || kind==TileKind::Ice;
}
}

void place_chasms(Game& game,const FloorPlan& plan) {
    const int round=(game.run.floor-1)%4;
    const bool forest=forest_floor(game.run.floor);
    if (forest && (round<2 || random_u32(game)%4!=0)) return;
    int budget=forest ? 1 : 2+round/2;
    std::vector<std::size_t> candidates;
    for (std::size_t i=0;i<plan.rooms.size();++i) if (fissure_room(plan.rooms[i].role)) candidates.push_back(i);
    for (std::size_t i=candidates.size();i>1;--i) std::swap(candidates[i-1],candidates[random_u32(game)%i]);
    for (auto index:candidates) {
        if (budget==0) break;
        const RoomPlan& room=plan.rooms[index];
        const bool horizontal=random_u32(game)%2==0;
        const Cell along=horizontal ? Cell{1,0} : Cell{0,1};
        const Cell across{along.y,along.x};
        const int reach=forest ? 3 : (horizontal ? room.half_width : room.half_height)-1;
        const int bank=(random_u32(game)%2==0 ? -1 : 1)*3;
        const int width=forest ? 0 : 1;
        int bend=0;
        std::vector<std::pair<Cell,Tile>> changed;
        for (int run=-reach;run<=reach;++run) {
            if (run%3==0) bend=std::clamp(bend+static_cast<int>(random_u32(game)%3)-1,-1,1);
            for (int side=-width;side<=width;++side) {
                const Cell cell=room.center+Cell{along.x*run+across.x*(bank+bend+side),along.y*run+across.y*(bank+bend+side)};
                Tile* tile=game.stage.at(cell);
                if (!tile || !dry(tile->kind) || plan.protected_cell(cell) || tile->prop.kind!=PropKind::None ||
                    std::abs(cell.x-room.center.x)>=room.half_width || std::abs(cell.y-room.center.y)>=room.half_height ||
                    distance(cell,game.run.spawn)<8 || entity_at(game,cell,false)>=0) continue;
                changed.push_back({cell,*tile});*tile={TileKind::Chasm};
            }
        }
        // Tiny failed cuts read as texture noise. Keep the original terrain.
        if (changed.size()<static_cast<std::size_t>(forest ? 3 : 8)) {
            for (const auto& [cell,tile]:changed) *game.stage.at(cell)=tile;
        } else --budget;
    }
}
