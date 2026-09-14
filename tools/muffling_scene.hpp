#pragma once
#include "../src/item_attribute.hpp"

// DISPLAY: Remaining wraps, a worn wrapped gun, and an untouched next target.
inline void arrange_muffling_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    player.cell = {20,16}; player.facing = {1,0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::MufflingFelt));
    insert_item(player.inventory,make_item(ItemKind::Musket));
    insert_item(player.inventory,make_item(ItemKind::Rake,1,ItemAttribute::Durable));
    insert_item(player.inventory,make_item(ItemKind::Bow));
    insert_item(player.inventory,make_item(ItemKind::Fist));
    player.inventory.slots[1].muffled_uses = 3;
    player.inventory.slots[2].muffled_uses = 6;
    player.inventory.slots[4].muffled_uses = 1;
    player.inventory.selected = 1;
    cosmetics = {}; cosmetics.camera = {20,16}; cosmetics.camera_ready = true;
    game.tick = 60;
}
