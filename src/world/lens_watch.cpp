#include "lens_watch.hpp"
#include "../entities/lens_warden.hpp"

#include <cstdlib>

namespace {

bool station_space(const Game& game, const RoomPlan& room, Cell cell) {
    if (std::abs(cell.x - room.center.x) >= room.half_width ||
        std::abs(cell.y - room.center.y) >= room.half_height) return false;
    for (const Entity& actor : game.entities)
        if ((actor.kind == EntityKind::Player || actor.kind == EntityKind::Key ||
            actor.kind == EntityKind::Switch || actor.kind == EntityKind::Door || actor.kind == EntityKind::Exit) &&
            distance(actor.cell, cell) <= 2) return false;
    return true;
}

} // namespace

Handle populate_lens_watch(Game& game, const RoomPlan& room) {
    constexpr Cell sides[]{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    // STATIONS: Fit the full lamp-to-mirror segment; never insert cover on required routes.
    for (int attempt = 0; attempt < 16; ++attempt) {
        const Cell stand = room.center + Cell{
            static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_width * 2 + 1)) - room.half_width,
            static_cast<int>(random_u32(game) % static_cast<unsigned int>(room.half_height * 2 + 1)) - room.half_height};
        for (Cell direction : sides) {
            const Cell mirror = stand + direction, side{-direction.y, direction.x};
            const Cell lamp = mirror + Cell{side.x * 3, side.y * 3};
            if (!station_space(game, room, stand) || !station_space(game, room, lamp) || !station_space(game, room, mirror)) continue;
            const Handle watch = spawn_lens_watch(game, stand, lamp, mirror);
            if (get_entity(game, watch)) return watch;
        }
    }
    return {};
}
