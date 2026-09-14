#pragma once
#include "../src/entities/ice_mason.hpp"
#include "../src/props/ice_cover.hpp"
#include "../src/props/interaction.hpp"

// POSES: Display work phases and cover damage without advancing AI or player input.
inline void arrange_mason_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 18; ++y)
        for (int x = 12; x <= 29; ++x)
            *game.stage.at({x, y}) = {y < 12 ? TileKind::Ice : TileKind::Snow, 0, 0};
    player.cell = {21, 15};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {12, 1700, {205, 225, 230}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Pickaxe));
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule, 3));
    const MasonPhase phases[]{MasonReady, MasonCut, MasonBuild, MasonSwing};
    const Sprite sprites[]{Sprite::MasonCarry, Sprite::MasonCut, Sprite::MasonBuild, Sprite::MasonJab};
    for (int i = 0; i < 4; ++i) {
        Entity* mason = get_entity(game, spawn_entity(game, EntityKind::IceMason, {15 + i * 4, 12}));
        mason->label_a = phases[i];
        mason->sprite = sprites[i];
        mason->timer_a = 16;
        mason->counter_a = i == 1 ? 0 : 1;
        mason->facing = {0, -1};
        mason->point_a = mason->cell;
        mason->point_b = mason->cell + mason->facing;
    }
    for (int i = 0; i < 4; ++i) {
        const Cell cell{15 + i * 4, 16};
        place_ice_cover(game, cell);
        if (i == 1) game.stage.at(cell)->prop.hp = 12;
        if (i == 2) game.stage.at(cell)->prop.growth_ticks = 60;
        if (i == 3) hit_prop(game, cell, 35, cell);
    }
    cosmetics.debris = {};
    prepare_debris(cosmetics.debris, game.stage);
    game.tick = 42;
    cosmetics.camera = {21, 14};
    cosmetics.camera_ready = true;
}
