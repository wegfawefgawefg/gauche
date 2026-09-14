#include "interaction.hpp"
#include "../world/route.hpp"
#include "../world/ice_terrain.hpp"

#include <cstdlib>

namespace {

PropKind room_prop(Game& game, RoomRole role) {
    const unsigned int roll = random_u32(game);
    if (ice_floor(game.run.floor) && role == RoomRole::WeatherStation) return PropKind::WeatherVane;
    if (ice_floor(game.run.floor) && role == RoomRole::FishingHut) return PropKind::FishingCreel;
    if (ice_floor(game.run.floor) && role == RoomRole::Observatory)
        return roll % 3 == 0 ? PropKind::LensCase : roll % 3 == 1 ? PropKind::CrystalLens : PropKind::MirrorShard;
    if (ice_floor(game.run.floor))
        return role == RoomRole::Bathhouse || role == RoomRole::Shrine ?
            PropKind::ClayPot : PropKind::Crate;
    switch (role) {
    case RoomRole::Thicket: return roll % 3 == 0 ? PropKind::Puffball : PropKind::TallGrass;
    case RoomRole::Brook: return roll % 2 == 0 ? PropKind::Fern : PropKind::Twigs;
    case RoomRole::Ruins: case RoomRole::Shrine:
        return roll % 3 == 0 ? PropKind::ClayPot : PropKind::Leaves;
    case RoomRole::Den: return roll % 3 == 0 ? PropKind::RottenLog : PropKind::Twigs;
    case RoomRole::Secret: case RoomRole::Cache: case RoomRole::Workshop:
        return roll % 3 == 0 ? PropKind::Crate : PropKind::Twigs;
    case RoomRole::Orchard: return roll % 5 == 0 ? PropKind::Nest : PropKind::Leaves;
    default: return roll % 3 == 0 ? PropKind::Fern : PropKind::Leaves;
    }
}

bool suitable(const Game& game, const FloorPlan& plan, Cell cell, bool blocking) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr || !walkable(*tile) || tile->kind == TileKind::Lava || tile->kind == TileKind::ShallowWater ||
        tile->kind == TileKind::Spring || tile->kind == TileKind::IceHole ||
        tile->prop.kind != PropKind::None || plan.protected_cell(cell)) return false;
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::None) continue;
        if (entity.cell == cell) return false;
        if ((entity.kind == EntityKind::GroundItem || entity.kind == EntityKind::Key ||
             entity.kind == EntityKind::Switch || entity.kind == EntityKind::Exit ||
             entity.kind == EntityKind::Door || entity.kind == EntityKind::Player) &&
            distance(cell, entity.cell) <= 2) return false;
    }
    // CLUTTER: Blocking scraps get breathing room; plants can form loose clusters.
    if (blocking)
        for (int y = -1; y <= 1; ++y)
            for (int x = -1; x <= 1; ++x)
                if (prop_blocks(game.stage.at_or_border(cell + Cell{x, y}).prop)) return false;
    return true;
}

} // namespace

void scatter_room_props(Game& game, const FloorPlan& plan) {
    const bool cold = ice_floor(game.run.floor);
    if (game.run.floor > 4 && !cold) return;
    for (const RoomPlan& room : plan.rooms) {
        // STASH: A few snowy lumps hide regional supplies off the mandatory paths.
        if (cold && (room.role == RoomRole::IceQuarry || room.role == RoomRole::Reservoir))
            for (int attempt = 0; attempt < 8; ++attempt) {
                const Cell cell = room.center + Cell{
                    static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_width * 2)) - room.half_width,
                    static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_height * 2)) - room.half_height};
                const Tile* tile = game.stage.at(cell);
                if (tile && tile->kind == TileKind::Snow && suitable(game, plan, cell, false)) {
                    place_prop(game.stage, cell, PropKind::SnowCache);
                    break;
                }
            }
        if (cold && room.role != RoomRole::WeatherStation && room.role != RoomRole::FishingHut && room.role != RoomRole::Shelter &&
            room.role != RoomRole::Bathhouse && room.role != RoomRole::Cache &&
            room.role != RoomRole::Observatory && room.role != RoomRole::Shrine) continue;
        const int patches = room.role == RoomRole::Thicket ? 7 : 3;
        for (int patch = 0; patch < patches; ++patch) {
            const Cell anchor = room.center + Cell{
                static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_width * 2)) - room.half_width,
                static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_height * 2)) - room.half_height};
            for (int piece = 0; piece < (cold ? 1 : 5); ++piece) {
                const Cell cell = anchor + Cell{static_cast<int>(random_u32(game) % 5) - 2,
                                                static_cast<int>(random_u32(game) % 5) - 2};
                if (std::abs(cell.x - room.center.x) > room.half_width ||
                    std::abs(cell.y - room.center.y) > room.half_height) continue;
                const PropKind kind = room_prop(game, room.role);
                if (suitable(game, plan, cell, prop_spec(kind).blocking))
                    place_prop(game.stage, cell, kind, static_cast<std::uint8_t>(random_u32(game)));
            }
        }
    }
}
