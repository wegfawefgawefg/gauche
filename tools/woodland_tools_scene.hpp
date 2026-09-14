#pragma once

#include "../src/props/growth.hpp"

// Static presentation fixture: plant stages and carried tool information.
inline void arrange_woodland_tools(Game& game, Cosmetics& cosmetics, Entity& player) {
    cosmetics = {};
    cosmetics.camera = {13.5F, 10.5F};
    cosmetics.camera_ready = true;
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles)
        if (walkable(tile.kind)) tile.prop = {};
    player.scorch_ticks = 0;
    player.cell = {13, 11};
    player.facing = {1, 0};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::Buckler, ItemKind::ResinGlue,
                         ItemKind::DiggingClaws, ItemKind::SeedBag, ItemKind::LanternSeed})
        insert_item(player.inventory, make_item(kind));
    player.inventory.slots[1].durability = 9;
    player.inventory.selected = 3;
    place_prop(game.stage, {10, 10}, PropKind::Shoot);
    game.stage.at({10, 10})->prop.growth_ticks = 170;
    place_prop(game.stage, {12, 10}, PropKind::Shoot);
    game.stage.at({12, 10})->prop.growth_ticks = 60;
    place_prop(game.stage, {14, 10}, PropKind::RootCover);
    place_prop(game.stage, {17, 10}, PropKind::LanternPlant);
    spawn_entity(game, EntityKind::LanternMoth, {17, 12});
}
