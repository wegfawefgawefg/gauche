#pragma once
#include "../src/projectiles/projectile.hpp"
#include "../src/particles/system.hpp"

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
