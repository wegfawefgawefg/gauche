#pragma once

#include "woodland_tools_scene.hpp"

// STATIC: Compare worn and Durable lamps; inspect a separate dropped emitter.
inline void arrange_shields(Game& game, Cosmetics& cosmetics, Entity& player, bool world) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::ShieldLantern));
    insert_item(player.inventory, make_item(ItemKind::ShieldLantern, 1, ItemAttribute::Durable));
    insert_item(player.inventory, make_item(ItemKind::ResinGlue));
    player.inventory.slots[1].durability = 11;
    player.inventory.selected = 1;
    player.block_ticks = 12;
    player.guard_slot = player.inventory.selected;
    if (!world) return;
    // Isolate the two real item lights so their footprint can be inspected.
    player.light = {};
    game.run.roof_light_count = 0;
    Entity* dropped = get_entity(game, spawn_entity(game, EntityKind::GroundItem, {20, 12}));
    dropped->ground_item = make_item(ItemKind::ShieldLantern);
    dropped->sprite = Sprite::ShieldLantern;
}
