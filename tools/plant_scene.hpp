#pragma once

#include "../src/game.hpp"
#include "../src/particles/system.hpp"

inline void arrange_plant_scene(Game& game, Cosmetics& cosmetics) {
    game = {};
    cosmetics = {};
    game.started = true;
    game.tick = 120;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    player_state(game,0).online = true;
    game.stage.width = 28; game.stage.height = 24;
    game.stage.tiles.assign(28*24, {TileKind::Wall, 100, 0});
    for (int y = 4; y < 20; ++y)
        for (int x = 4; x < 24; ++x) *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.run.spawn = {14, 12};
    player_state(game,0).controlled = spawn_entity(game, EntityKind::Player, game.run.spawn);
    cosmetics.camera = {14.5F, 12.0F}; cosmetics.camera_ready = true;
    const Handle root_handle = spawn_entity(game, EntityKind::RootTurret, {11, 11});
    Entity& root = *get_entity(game, root_handle);
    root.label_a = 1; root.timer_a = 24; root.facing = {1, 0};
    root.sprite = Sprite::RootTurretCoiled;
    Entity& guard = *get_entity(game, spawn_entity(game, EntityKind::BrambleGuard, {12, 12}));
    guard.entity_a = root_handle; guard.facing = {1, 0}; guard.block_ticks = 2; guard.guard_slot = guard.inventory.selected;
    Entity& lashing = *get_entity(game, spawn_entity(game, EntityKind::RootTurret, {11, 15}));
    lashing.label_a = 2; lashing.timer_a = 89; lashing.facing = {1, 0};
    Entity& swinging = *get_entity(game, spawn_entity(game, EntityKind::BrambleGuard, {16, 14}));
    swinging.label_a = 1; swinging.timer_a = 15; swinging.facing = {-1, 0};
    swinging.sprite = Sprite::BrambleGuardSwing; swinging.point_b = {15, 14};
}
