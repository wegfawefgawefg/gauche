#pragma once

#include "../src/game.hpp"
#include "../src/particles/system.hpp"

inline void arrange_footprint_scene(Game& game, Cosmetics& cosmetics) {
    game = {}; cosmetics = {};
    game.started = true; game.tick = 120;
    game.run.floor = 1; game.run.phase = RunPhase::Playing; game.run.online[0] = true;
    game.stage.width = 28; game.stage.height = 24;
    game.stage.tiles.assign(28*24, {TileKind::Wall, 100, 0});
    for (int y = 4; y < 20; ++y)
        for (int x = 4; x < 24; ++x) *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.run.spawn = {11, 12};
    game.players[0] = spawn_entity(game, EntityKind::Player, game.run.spawn);
    Entity& player = *get_entity(game, game.players[0]);
    cosmetics.frame_alpha = 1;
    update_cosmetics(cosmetics, game, player.cell);
    // PRESENTATION ONLY: Feed recorded cardinal positions into the pose/footprint
    // observer. No game stepping, input, AI or networking is run by this capture.
    for (int frame = 0; frame < 16; ++frame) {
        ++game.tick;
        if (frame%7 == 0) { ++player.cell.x; player.facing = {1, 0}; }
        update_cosmetics(cosmetics, game, player.cell);
    }
    cosmetics.camera = {13.5F, 12}; cosmetics.camera_ready = true;
}
