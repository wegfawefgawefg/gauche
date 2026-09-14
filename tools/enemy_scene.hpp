#pragma once

#include "../src/game.hpp"
#include "../src/particles/system.hpp"

inline void arrange_enemy_scene(Game& game, Cosmetics& cosmetics) {
    game = {};
    cosmetics = {};
    game.started = true;
    game.tick = 120;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    game.run.online[0] = true;
    game.stage.width = 32;
    game.stage.height = 24;
    game.stage.tiles.assign(32 * 24, {TileKind::Wall, 100, 0});
    for (int y = 3; y < 21; ++y)
        for (int x = 3; x < 29; ++x) *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.players[0] = spawn_entity(game, EntityKind::Player, {15, 12});
    game.run.spawn = {15, 12};
    game.run.roof_light_count = 2;
    game.run.roof_lights[0] = {{10, 9}, {8, 1700, {240, 224, 176}}};
    game.run.roof_lights[1] = {{21, 13}, {8, 1700, {240, 224, 176}}};
    cosmetics.camera = {15.5F, 12.0F};
    cosmetics.camera_ready = true;
    const auto actor = [&](EntityKind kind, Cell cell, int phase, Cell direction) -> Entity& {
        Entity& enemy = *get_entity(game, spawn_entity(game, kind, cell));
        enemy.label_a = phase;
        enemy.timer_a = 24;
        enemy.facing = direction;
        return enemy;
    };
    actor(EntityKind::RootTurret, {9, 18}, 1, {0, -1}).sprite = Sprite::RootTurretCoiled;
    actor(EntityKind::BrambleGuard, {10, 16}, 1, {1, 0}).sprite = Sprite::BrambleGuardSwing;
    actor(EntityKind::Boar, {8, 7}, 1, {1, 0}).counter_a = 6;
    actor(EntityKind::ThornSnail, {17, 7}, 0, {1, 0});
    Entity& shell = actor(EntityKind::ThornSnail, {20, 7}, 1, {1, 0});
    shell.sprite = Sprite::ThornSnailClosed;
    shell.hard_blocker = true;
    actor(EntityKind::SporeToad, {9, 13}, 1, {1, 0}).sprite = Sprite::SporeToadSwollen;
    actor(EntityKind::LanternMoth, {20, 12}, 1, {1, 0});
    actor(EntityKind::Bear, {17, 17}, 1, {0, -1});
    actor(EntityKind::Wolf, {12, 17}, 1, {1, 0}).point_b = {13, 17};
    actor(EntityKind::Bat, {24, 17}, 1, {0, -1}).counter_a = 4;
    actor(EntityKind::CrateMimic, {24, 10}, 1, {0, 1}).sprite = Sprite::CrateMimic;
    for (Entity& enemy : game.entities)
        if (enemy.kind == EntityKind::CrateMimic || enemy.kind == EntityKind::BrambleGuard) enemy.point_b = enemy.cell + enemy.facing;
}
