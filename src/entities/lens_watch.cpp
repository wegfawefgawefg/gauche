#include "lens_warden.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"

namespace {

bool free_station_cell(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    return tile && buildable(tile->kind) && !surface_wet(*tile) && !tile->surface.fire_ticks &&
        (tile->prop.kind == PropKind::None || tile->prop.broken) && entity_at(game, cell, false) < 0;
}

} // namespace

Handle spawn_lens_watch(Game& game, Cell stand, Cell lamp, Cell mirror) {
    if (stand == lamp || distance(stand, mirror) != 1 || distance(lamp, mirror) != 3 ||
        (lamp.x != mirror.x && lamp.y != mirror.y) || !free_station_cell(game, stand) ||
        !free_station_cell(game, lamp) || !free_station_cell(game, mirror)) return {};
    const Cell direction = cardinal_toward(lamp, mirror, {1, 0});
    for (int i = 1; i < 3; ++i)
        if (!free_station_cell(game, lamp + Cell{direction.x * i, direction.y * i})) return {};
    const Handle handle = spawn_entity(game, EntityKind::LensWarden, stand);
    Entity* warden = get_entity(game, handle);
    if (!warden) return {};
    // ALLOCATION: No scenery is written until the actor slot and full station fit.
    game.stage.at(lamp)->prop = {};
    game.stage.at(mirror)->prop = {};
    place_prop(game.stage, lamp, PropKind::BeamLamp);
    const Cell reflected{-direction.y, -direction.x};
    const Cell toward_stand = cardinal_toward(mirror, stand, {1, 0});
    place_prop(game.stage, mirror, PropKind::MirrorShard, reflected == toward_stand ? 0 : 1);
    warden->point_a = lamp;
    warden->point_b = mirror;
    warden->facing = cardinal_toward(stand, mirror, {1, 0});
    return handle;
}
