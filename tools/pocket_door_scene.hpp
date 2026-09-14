#pragma once

#include "woodland_tools_scene.hpp"
#include "../src/particles/templates.hpp"

// STATIC: Fresh/half-used cards and linked thresholds on either side of ordinary cover.
inline void arrange_pocket_doors(Game& game, Cosmetics& cosmetics, Entity& player, bool world) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::PocketDoor));
    insert_item(player.inventory, make_item(ItemKind::PocketDoor));
    player.inventory.selected = 1;
    Item& half = player.inventory.slots[2];
    half.uses = 1;
    half.anchor = spawn_entity(game, EntityKind::PocketDoor, {15, 13});
    if (!world) return;
    const Handle first = spawn_entity(game, EntityKind::PocketDoor, {15, 11});
    const Handle second = spawn_entity(game, EntityKind::PocketDoor, {18, 11});
    get_entity(game, first)->entity_a = second;
    get_entity(game, second)->entity_a = first;
    for (Handle handle : {first, second}) {
        Entity* door = get_entity(game, handle);
        door->fixture_open = true;
        door->sprite = Sprite::PocketThresholdOpen;
        door->light = {3, 280, {125, 177, 237}};
        spawn_sound_effect(cosmetics, {SoundId::PocketTravel, door->cell, game.tick, 0, true}, 719);
    }
    for (int y = 9; y <= 12; ++y)
        *game.stage.at({16, y}) = {TileKind::Wall, 100, 0, 100, BreakRule::DigRequired, 2};
}
