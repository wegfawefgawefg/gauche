#pragma once

#include "woodland_tools_scene.hpp"
#include "../src/props/scarecrow.hpp"
#include "../src/debris/system.hpp"

// STATIC: Ward silhouette, broken straw/cloth and ordinary versus Big item cards.
inline void arrange_decoys(Game& game, Cosmetics& cosmetics, Entity& player, bool world) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::Scarecrow, 2));
    insert_item(player.inventory, make_item(ItemKind::Scarecrow, 2, ItemAttribute::Big));
    player.inventory.selected = 1;
    if (!world) return;
    game.run.roof_light_count = 0;
    place_prop(game.stage, {15, 11}, PropKind::Scarecrow, 4);
    place_prop(game.stage, {17, 13}, PropKind::Scarecrow, 5);
    game.stage.at({17, 13})->prop.hp = 15;
    place_prop(game.stage, {12, 13}, PropKind::Scarecrow, 4);
    game.stage.at({12, 13})->prop.broken = true;
    game.stage.at({12, 13})->prop.hp = 0;
    scatter_prop_debris(cosmetics.debris, {12, 13}, PropKind::Scarecrow, 43, true);
    spawn_entity(game, EntityKind::Owl, {20, 11});
    spawn_entity(game, EntityKind::CarrionCrow, {19, 10});
    spawn_entity(game, EntityKind::Bunny, {10, 12});
}
