#pragma once
#include "../src/combat/beams.hpp"
#include "../src/particles/gunfire.hpp"

// DISPLAY: Fixed optics and their pure traced route, without stepping combat or AI.
inline void arrange_optics_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool beams) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 6; y <= 23; ++y)
        for (int x = 8; x <= 33; ++x)
            *game.stage.at({x, y}) = {TileKind::Empty, 0, 0};
    player.cell = {16, 16};
    player.facing = {1, 0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {15, 1500, {212, 226, 223}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::LensCarbine));
    insert_item(player.inventory, make_item(ItemKind::MirrorShard, 3));
    insert_item(player.inventory, make_item(ItemKind::CrystalLens));
    player.inventory.selected = 0;
    place_prop(game.stage, {21, 16}, PropKind::MirrorShard, 0);
    place_prop(game.stage, {21, 12}, PropKind::CrystalLens);
    place_prop(game.stage, {25, 12}, PropKind::MirrorShard, 1);
    place_prop(game.stage, {17, 12}, PropKind::MirrorShard, 0);
    place_prop(game.stage, {17, 19}, PropKind::CrystalLens);
    place_prop(game.stage, {25, 20}, PropKind::Crate);
    cosmetics = {};
    cosmetics.camera = {21, 16};
    cosmetics.camera_ready = true;
    game.tick = 60;
    game.shot_count = 0;
    if (!beams) return;
    const BeamTrace trace = trace_beam(game, player.cell, player.facing, 18, 12, false);
    for (int i = 0; i < trace.count; ++i) {
        const BeamCell hit = trace.cells[static_cast<std::size_t>(i)];
        game.shots[static_cast<std::size_t>(game.shot_count++)] =
            {hit.from, hit.cell, hit.stop, i == 0, false, false, true};
    }
    observe_gunfire(cosmetics, game, player.cell);
}
