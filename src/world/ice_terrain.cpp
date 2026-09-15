#include "ice_terrain.hpp"

#include <cstdlib>

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
    case RoomRole::CliffPath: return ax > 2 || ay > 2 ? TileKind::Snow : TileKind::Empty;
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
    // MATERIAL: Ice walls carry a gameplay material, independent of their displayed sprite.
    for (Tile& tile : game.stage.tiles)
        if (tile.kind == TileKind::Wall && tile.material == TileMaterial::Stone) tile.material = TileMaterial::Ice;
    for (const RoomPlan& room : plan.rooms) {
        if (room.role != RoomRole::Reservoir && room.role != RoomRole::FishingHut) continue;
        Cell first{};
        int holes = 0;
        // ACCESS: Two shallow openings on the same side of the reserved dry crossing.
        // Holes never replace a mandatory path, a gate, a wall or the spawn area.
        for (int y = -2; y >= -room.half_height + 1 && holes < 2; --y)
            for (int x = -2; x >= -room.half_width + 1 && holes < 2; --x) {
                const Cell cell = room.center + Cell{x, y};
                Tile* tile = game.stage.at(cell);
                if (tile == nullptr || tile->kind != TileKind::Ice || plan.protected_cell(cell)) continue;
                if (holes > 0 && distance(first, cell) < 2) continue;
                *tile = {TileKind::IceHole, 0, 0};
                if (holes == 0) first = cell;
                ++holes;
            }
    }
}
