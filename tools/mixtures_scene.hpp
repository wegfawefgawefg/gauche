#pragma once

#include "woodland_tools_scene.hpp"
#include "../src/surfaces/interaction.hpp"
#include "../src/surfaces/render.hpp"
#include "../src/projectiles/projectile.hpp"

// Static material layers, fuse and item details; no step_game or AI execution.
inline void arrange_mixtures(Game& game, Cosmetics& cosmetics, Entity& player, bool world, bool big) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::StinkBomb, ItemKind::RottenFruit, ItemKind::PitchBomb, ItemKind::WaterFlask})
        insert_item(player.inventory, make_item(kind, kind == ItemKind::RottenFruit ? 4 : 1));
    player.inventory.slots[3] = make_item(ItemKind::PitchBomb, 2, big ? ItemAttribute::Big : ItemAttribute::None);
    player.inventory.selected = 3;
    player.health = 68;
    player.vitals.nausea = world ? 165 : 0;
    player.vitals.nausea_wait = world ? 45 : 0;
    for (Cell cell : {Cell{10, 10}, {11, 10}, {10, 11}})
        game.stage.at(cell)->surface.scent_ticks = 500;
    for (Cell cell : {Cell{15, 10}, {16, 10}, {15, 11}})
        pour_surface(game, cell, LiquidKind::Rot, 420);
    for (Cell cell : {Cell{15, 13}, {16, 13}, {17, 13}}) {
        pour_surface(game, cell, LiquidKind::Sap, 900);
        ignite_surface(game, cell);
        game.stage.at(cell)->surface.liquid_ticks = 900;
    }
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, {14, 11}));
    shot->label_a = static_cast<int>(ProjectileKind::Mixture);
    shot->sprite = Sprite::PitchBombLit; shot->ground_item = player.inventory.slots[3];
    shot->counter_a = 0; shot->attack_interval = 6; shot->timer_a = 60; shot->facing = {1, 0};
    shot->light = {2, 170, {255, 154, 64}};
    game.tick = 120;
    observe_surfaces(cosmetics, game, player.cell);
}
