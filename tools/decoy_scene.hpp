#pragma once

#include "woodland_tools_scene.hpp"
#include "../src/props/scarecrow.hpp"
#include "../src/debris/system.hpp"

// STATIC: Ward silhouette, broken straw/cloth and ordinary versus Big item cards.
inline void arrange_decoys(Game& game, Cosmetics& cosmetics, Entity& player, bool world, bool straw = false) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    const ItemKind item = straw ? ItemKind::StrawDecoy : ItemKind::Scarecrow;
    const PropKind prop = straw ? PropKind::StrawDecoy : PropKind::Scarecrow;
    const int radius = straw ? 6 : 4;
    insert_item(player.inventory, make_item(item, 2));
    insert_item(player.inventory, make_item(item, 2, ItemAttribute::Big));
    player.inventory.selected = 1;
    if (!world) return;
    game.run.roof_light_count = 0;
    place_prop(game.stage, {15, 11}, prop, radius);
    place_prop(game.stage, {17, 13}, prop, radius + 1);
    game.stage.at({17, 13})->prop.hp = 15;
    place_prop(game.stage, {12, 13}, prop, radius);
    game.stage.at({12, 13})->prop.broken = true;
    game.stage.at({12, 13})->prop.hp = 0;
    scatter_prop_debris(cosmetics.debris, {12, 13}, prop, 43, true);
    spawn_entity(game, EntityKind::Owl, {20, 11});
    spawn_entity(game, EntityKind::CarrionCrow, {19, 10});
    spawn_entity(game, EntityKind::Bunny, {10, 12});
}
