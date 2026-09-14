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
        if (y < -3 && x > 2 && x + ay < room.half_width + room.half_height - 6)
            return TileKind::Water;
        return y == -2 && x < -2 ? TileKind::ShallowWater : TileKind::Ice;
    case RoomRole::IceQuarry:
        return (room.mirrored ? x > 2 : x < -2) ? TileKind::Ice : TileKind::Snow;
    case RoomRole::Bathhouse:
        return ax > 2 && ay > 2 ? TileKind::ShallowWater : TileKind::Ruin;
    case RoomRole::FishingHut:
        return y < -2 ? TileKind::Ice : TileKind::Ruin;
    case RoomRole::EchoTunnel: return ax > room.half_width - 2 ? TileKind::Ice : TileKind::Empty;
    case RoomRole::Shelter: case RoomRole::Observatory:
    case RoomRole::Shrine: case RoomRole::Cache: case RoomRole::Secret:
        return TileKind::Ruin;
    default: return ax + ay > room.half_width ? TileKind::Snow : TileKind::Empty;
    }
}
