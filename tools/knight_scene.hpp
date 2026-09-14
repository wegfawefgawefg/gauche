#pragma once
#include "../src/entities/mirror_knight.hpp"
#include "../src/combat/beams.hpp"
#include "../src/particles/gunfire.hpp"

// DISPLAY: Fixed stance samples and a pure beam path. No combat or AI is stepped.
inline void arrange_knight_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool beam) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 7; y <= 22; ++y)
        for (int x = 9; x <= 32; ++x)
            *game.stage.at({x, y}) = {TileKind::Empty, 0, 0};
    player.cell = {17, 17}; player.facing = {1, 0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {16, 1650, {212, 226, 223}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::LensCarbine));
    player.inventory.selected = 0;
    const int phases[]{KnightAdvance, KnightRaise, KnightGuard, KnightSwing, KnightRecover, KnightGuard, KnightGuard, KnightGuard};
    const Sprite sprites[]{Sprite::MirrorKnight, Sprite::KnightRaise, Sprite::KnightGuard, Sprite::KnightSwing,
        Sprite::KnightRecover, Sprite::KnightGuard, Sprite::KnightGuard, Sprite::KnightGuard};
    const Cell cells[]{{14, 12}, {18, 12}, {22, 12}, {26, 12}, {14, 19}, {18, 19}, {22, 19}, {26, 19}};
    const Cell facings[]{{1, 0}, {1, 0}, {-1, 0}, {1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 0}};
    for (int i = 0; i < (beam ? 1 : 8); ++i) {
        Entity* knight = get_entity(game, spawn_entity(game, EntityKind::MirrorKnight, beam ? Cell{23, 17} : cells[i]));
        knight->label_a = beam ? KnightGuard : phases[i];
        knight->sprite = beam ? Sprite::KnightGuard : sprites[i];
        knight->timer_a = 12;
        knight->point_a = knight->cell;
        knight->facing = beam ? Cell{-1, 0} : facings[i];
        knight->point_b = knight->cell + knight->facing;
    }
    cosmetics = {};
    cosmetics.camera = {20, 16}; cosmetics.camera_ready = true;
    game.tick = 60; game.shot_count = 0;
    if (!beam) return;
    const BeamTrace trace = trace_beam(game, player.cell, player.facing, 18, 12, false);
    for (int i = 0; i < trace.count; ++i) {
        const BeamCell& hit = trace.cells[static_cast<std::size_t>(i)];
        game.shots[static_cast<std::size_t>(game.shot_count++)] = {hit.from, hit.cell, hit.stop, i == 0, false, false, true};
    }
    observe_gunfire(cosmetics, game, player.cell);
}
