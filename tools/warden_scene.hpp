#pragma once
#include "../src/entities/lens_warden.hpp"
#include "../src/particles/gunfire.hpp"

// DISPLAY: A fixed station and pure traced shot; no AI, combat or input is stepped.
inline void arrange_warden_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool fired) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 7; y <= 23; ++y)
        for (int x = 9; x <= 34; ++x)
            *game.stage.at({x, y}) = {TileKind::Empty, 0, 0};
    player.cell = {25, 16}; player.facing = {-1, 0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {13, 1550, {211, 229, 225}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::MirrorShard, 2));
    Entity* warden = get_entity(game, spawn_lens_watch(game, {19, 16}, {20, 13}, {20, 16}));
    warden->label_a = WardenCharge;
    warden->timer_a = 24;
    warden->label_b = 1;
    warden->sprite = Sprite::WardenCharge;
    game.stage.at(warden->point_b)->prop.variant = 1;
    Entity* turning = get_entity(game, spawn_lens_watch(game, {15, 20}, {16, 17}, {16, 20}));
    turning->label_a = WardenTurn; turning->label_b = 1;
    turning->timer_a = 15; turning->sprite = Sprite::WardenTurn;
    cosmetics = {};
    cosmetics.camera = {21, 16}; cosmetics.camera_ready = true;
    game.tick = 60; game.shot_count = 0;
    if (!fired) return;
    const BeamTrace trace = warden_beam(game, *warden);
    for (int i = 0; i < trace.count; ++i) {
        const BeamCell& hit = trace.cells[static_cast<std::size_t>(i)];
        game.shots[static_cast<std::size_t>(game.shot_count++)] = {hit.from, hit.cell, hit.stop, i == 0, false, false, true};
    }
    warden->label_a = WardenRecover; warden->sprite = Sprite::WardenRecover;
    observe_gunfire(cosmetics, game, player.cell);
}
