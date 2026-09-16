#include "ice_terrain.hpp"
#include "ice_material.hpp"

#include <cstdlib>
#include <algorithm>

TileKind ice_room_floor(const RoomPlan& room, int x, int y) {
    const int ax = std::abs(x), ay = std::abs(y);
    // BANKS: Required paths are reserved by the carver before this function.
    switch (room.role) {
    case RoomRole::Reservoir:
        // SHORE: One broad frozen basin, open water at its far end, dry southern bank.
        if (y > 1) return x + y > room.half_width ? TileKind::Snow : TileKind::Empty;
        if (ax >= room.half_width - 1 || ay >= room.half_height - 1 ||
            ax + ay > room.half_width + room.half_height - 4) return TileKind::Snow;
        // SPANS: Three-cell channels admit portable bridges; the frozen banks remain.
        if (y < -3 && x > 2 && x <= 5 && x + ay < room.half_width + room.half_height - 6)
            return TileKind::Water;
        if (y < -1 && x > 2 && x <= 5) return TileKind::ShallowWater;
        return y == -2 && x < -2 ? TileKind::ShallowWater : TileKind::Ice;
    case RoomRole::IceQuarry:
        return (room.mirrored ? x > 2 : x < -2) ? TileKind::Ice : TileKind::Snow;
    case RoomRole::Bathhouse:
        return ax > 2 && ax <= 5 && ay > 2 && ay < room.half_height - 1 ? TileKind::ShallowWater : TileKind::Ruin;
    case RoomRole::FishingHut:
        return y < -2 ? TileKind::Ice : TileKind::Ruin;
    case RoomRole::CrystalGallery: return ax > room.half_width-2 ? TileKind::Ice : TileKind::Ruin;
    case RoomRole::BoilerGallery: return TileKind::Ruin;
    case RoomRole::ServicePassage: case RoomRole::Chapel: return TileKind::Ruin;
    case RoomRole::MemorialCourt: return ax > room.half_width-2 || ay > room.half_height-2 ? TileKind::Snow : TileKind::Ruin;
    case RoomRole::CliffPath:
        // Side shelves punish careless throws/charges without icing the reserved route.
        if (ax>2 && ay<=2) return TileKind::Ice;
        return ax > 2 || ay > 2 ? TileKind::Snow : TileKind::Empty;
    case RoomRole::WeatherStation: return ax > 2 || ay > 2 ? TileKind::Snow : TileKind::Ruin;
    case RoomRole::EchoTunnel: return ax > room.half_width - 2 ? TileKind::Ice : TileKind::Empty;
    case RoomRole::Shelter: case RoomRole::Observatory:
    case RoomRole::Shrine: case RoomRole::Cache: case RoomRole::Secret:
        return TileKind::Ruin;
    default: return ax + ay > room.half_width ? TileKind::Snow : TileKind::Empty;
    }
}

void place_ice_terrain(Game& game, const FloorPlan& plan) {
    if (!ice_floor(game.run.floor)) return;
    place_ice_materials(game,plan);
    for (const RoomPlan& room : plan.rooms) {
        if (room.role != RoomRole::Reservoir && room.role != RoomRole::FishingHut) continue;
        std::vector<Cell> candidates,placed;
        // Choose among real icy banks instead of taking the first two cells of
        // a fixed upper-left scan. Required routes and authored thaw pockets win.
        for (int y=-room.half_height+1;y<room.half_height;++y)
            for (int x=-room.half_width+1;x<room.half_width;++x) {
                const Cell cell=room.center+Cell{x,y};
                const Tile* tile=game.stage.at(cell);
                if (tile && tile->kind==TileKind::Ice && !plan.protected_cell(cell)) candidates.push_back(cell);
            }
        for (std::size_t i=candidates.size();i>1;--i)
            std::swap(candidates[i-1],candidates[random_u32(game)%i]);
        const std::size_t desired=2+random_u32(game)%2;
        for (const Cell cell:candidates) {
            if (placed.size()==desired) break;
            if (std::any_of(placed.begin(),placed.end(),[cell](Cell old) {return distance(old,cell)<3;})) continue;
            *game.stage.at(cell)={TileKind::IceHole};placed.push_back(cell);
        }
    }
}
