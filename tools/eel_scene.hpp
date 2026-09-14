#pragma once
#include "../src/entities/glass_eel.hpp"
#include "../src/surfaces/conduction.hpp"
#include "../src/particles/electricity.hpp"

// DISPLAY: Arrange poses and a sampled circuit. No AI, input or combat is stepped.
inline void arrange_eel_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool arcs) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 19; ++y)
        for (int x = 11; x <= 30; ++x)
            *game.stage.at({x, y}) = {TileKind::Snow, 0, 0};
    for (int y = 9; y <= 13; ++y)
        for (int x = 13; x <= 27; ++x)
            *game.stage.at({x, y}) = {x > 23 ? TileKind::Water : TileKind::ShallowWater, 0, 0};
    for (int y = 9; y <= 13; ++y) *game.stage.at({20, y}) = {TileKind::Ice, 0, 0};
    player.cell = {19, 15};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {13, 1700, {212, 226, 223}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::EelBattery));
    insert_item(player.inventory, make_item(ItemKind::EelBattery, 1, ItemAttribute::Big));
    insert_item(player.inventory, make_item(ItemKind::EelBattery, 1, ItemAttribute::Durable));
    insert_item(player.inventory, make_item(ItemKind::ColdFlask, 3));
    player.inventory.selected = 1;
    const Sprite sprites[]{Sprite::GlassEel, Sprite::EelCharge, Sprite::EelSpent, Sprite::EelStranded};
    const Cell cells[]{{14, 11}, {18, 11}, {24, 11}, {27, 15}};
    for (int i = 0; i < 4; ++i) {
        Entity* eel = get_entity(game, spawn_entity(game, EntityKind::GlassEel, cells[i]));
        eel->sprite = sprites[i];
        eel->label_a = i == 1 ? EelCharge : i == 2 ? EelRest : EelSwim;
        eel->timer_a = 24;
        eel->point_a = eel->cell;
        eel->facing = {1, 0};
        if (i == 1) eel->self_light = {90, 152, 180};
    }
    cosmetics = {};
    cosmetics.camera = {20, 13};
    cosmetics.camera_ready = true;
    game.tick = 60;
    if (arcs) {
        const WetWave wave = wet_wave(game, {18, 11}, eel_shock_reach);
        spawn_electric_arc(cosmetics, {18, 11}, {18, 11}, 1251);
        for (int i = 1; i < wave.count; ++i) {
            const WetNode node = wave.nodes[static_cast<std::size_t>(i)];
            spawn_water_arc(cosmetics, wave.nodes[static_cast<std::size_t>(node.parent)].cell,
                node.cell, static_cast<std::uint64_t>(i + 1251));
        }
    }
}
