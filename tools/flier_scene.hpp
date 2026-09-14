#pragma once

#include "../src/game.hpp"
#include "../src/particles/system.hpp"

inline void arrange_flier_scene(Game& game, Cosmetics& cosmetics) {
    game = {}; cosmetics = {};
    game.started = true; game.tick = 120;
    game.run.floor = 1; game.run.phase = RunPhase::Playing; game.run.online[0] = true;
    game.stage.width = 28; game.stage.height = 24;
    game.stage.tiles.assign(28*24, {TileKind::Wall, 100, 0});
    for (int y = 4; y < 20; ++y)
        for (int x = 4; x < 24; ++x) *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.run.spawn = {14, 12};
    game.players[0] = spawn_entity(game, EntityKind::Player, game.run.spawn);
    cosmetics.camera = {14.5F, 12}; cosmetics.camera_ready = true;
    const auto actor = [&](EntityKind kind, Cell cell, int phase, Sprite sprite) -> Entity& {
        Entity& bird = *get_entity(game, spawn_entity(game, kind, cell));
        bird.label_a = phase; bird.timer_a = 24; bird.sprite = sprite;
        bird.facing = {1, 0}; bird.point_b = game.run.spawn; bird.counter_a = 4;
        return bird;
    };
    actor(EntityKind::Mosquito, {12, 10}, 1, Sprite::Mosquito);
    actor(EntityKind::Mosquito, {16, 10}, 3, Sprite::MosquitoFed);
    actor(EntityKind::Owl, {10, 12}, 1, Sprite::Owl);
    actor(EntityKind::Owl, {18, 12}, 2, Sprite::OwlFlying).point_b = {17, 13};
    actor(EntityKind::Woodpecker, {12, 15}, 1, Sprite::Woodpecker);
    actor(EntityKind::Woodpecker, {16, 15}, 2, Sprite::WoodpeckerDrilling);
}
