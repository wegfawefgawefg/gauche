#pragma once
#include "optics_scene.hpp"

// DISPLAY: Covered/uncovered optics and one stopped beam, without stepping gameplay.
inline void arrange_felt_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool beam) {
    arrange_optics_scene(game, cosmetics, player, false);
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::BlackFelt, 3));
    insert_item(player.inventory, make_item(ItemKind::LensCarbine));
    insert_item(player.inventory, make_item(ItemKind::MirrorShard, 2));
    player.inventory.selected = 1;
    for (int i = 0; i < 3; ++i) {
        const PropKind kind = i == 0 ? PropKind::MirrorShard : i == 1 ? PropKind::CrystalLens : PropKind::BeamLamp;
        place_prop(game.stage, {19 + i*3, 13}, kind);
        place_prop(game.stage, {19 + i*3, 16}, kind);
        game.stage.at({19 + i*3, 16})->prop.covered = true;
    }
    place_prop(game.stage, {22, 19}, PropKind::LensCase);
    scatter_prop_debris(cosmetics.debris, {25, 19}, PropKind::LensCase, 73, true);
    cosmetics.debris.ready = true;
    if (!beam) return;
    const BeamTrace trace = trace_beam(game, player.cell, player.facing, 18, 12, false);
    for (int i = 0; i < trace.count; ++i) {
        const BeamCell& hit = trace.cells[static_cast<std::size_t>(i)];
        game.shots[static_cast<std::size_t>(game.shot_count++)] =
            {hit.from, hit.cell, hit.stop, i == 0, false, false, true};
    }
    observe_gunfire(cosmetics, game, player.cell);
}
