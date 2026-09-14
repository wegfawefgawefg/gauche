#pragma once
#include "../src/projectiles/projectile.hpp"
#include "../src/particles/system.hpp"
#include "../src/particles/gunfire.hpp"

inline void arrange_projectile_scene(Game& game, Cosmetics& cosmetics) {
    game = {}; cosmetics = {};
    game.started = true; game.tick = 90;
    game.run.floor = 1; game.run.phase = RunPhase::Playing; game.run.online[0] = true;
    game.stage.width = 24; game.stage.height = 18;
    game.stage.tiles.assign(24 * 18, {TileKind::Wall, 100, 0});
    for (int y = 4; y < 14; ++y)
        for (int x = 4; x < 20; ++x) *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.run.spawn = {10, 9}; game.run.exit = {18, 9};
    game.players[0] = spawn_entity(game, EntityKind::Player, game.run.spawn);
    Entity& player = *get_entity(game, game.players[0]); player.owner = 0; player.facing = {1, 0};
    launch_projectile(game, game.players[0].slot, make_item(ItemKind::Bow), {1, 0}, 14);
    launch_projectile(game, game.players[0].slot, make_item(ItemKind::Bomb), {0, 1}, 3);
    for (Entity& shot : game.entities) {
        if (shot.kind != EntityKind::Projectile) continue;
        if (shot.label_a == static_cast<int>(ProjectileKind::Arrow)) {
            shot.cell = {13, 9}; shot.counter_a = 11; shot.timer_b = 1;
        } else { shot.cell = {11, 11}; shot.counter_a = 2; shot.timer_b = 2; }
    }
    spawn_entity(game, EntityKind::Bear, {16, 9});
    cosmetics.camera = {12, 9}; cosmetics.camera_ready = true;
}

inline void arrange_ballistics_scene(Game& game, Cosmetics& cosmetics) {
    arrange_projectile_scene(game, cosmetics);
    const int owner = game.players[0].slot;
    launch_projectile(game, owner, make_item(ItemKind::Crossbow), {0, -1}, 14);
    launch_projectile(game, owner, make_item(ItemKind::RocketLauncher), {1, 0}, 14);
    for (Entity& shot : game.entities) {
        if (shot.kind != EntityKind::Projectile) continue;
        if (shot.ground_item.kind == ItemKind::Crossbow) {
            shot.cell = {10, 6}; shot.counter_a = 11; shot.timer_b = 1;
        } else if (shot.ground_item.kind == ItemKind::RocketLauncher) {
            shot.cell = {14, 7}; shot.counter_a = 10; shot.timer_b = 1;
        }
    }
    prepare_debris(cosmetics.debris, game.stage);
    game.shots[0] = {{10, 9}, {6, 9}, true, true, true};
    game.shot_count = 1;
    observe_gunfire(cosmetics, game, {10, 9});
}
