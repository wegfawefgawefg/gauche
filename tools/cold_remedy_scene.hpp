#pragma once

#include "../src/items/cold_remedies.hpp"

// DISPLAY: Arrange actual item effects without advancing the simulation.
inline void arrange_cold_remedies(Game& game, Entity& player, bool status, bool poultice) {
    player.inventory = {};
    for (ItemKind kind : {ItemKind::WoolWrap, ItemKind::HotBroth, ItemKind::IcePoultice})
        insert_item(player.inventory, make_item(kind, 3));
    insert_item(player.inventory, make_item(ItemKind::HotBroth, 2, ItemAttribute::Restorative));
    insert_item(player.inventory, make_item(ItemKind::IcePoultice, 2, ItemAttribute::Restorative));
    insert_item(player.inventory, make_item(ItemKind::Fist));
    player.health = 41;
    player.burn_ticks = player.scorch_ticks = 0;
    player.vitals = {};
    player.freeze_ticks = 120;
    if (status) {
        const int slot = static_cast<int>(&player - game.entities.data());
        if (poultice) {
            player.inventory.selected = 2;
            player.scorch_ticks = 240;
            use_cold_remedy(game, slot);
        } else {
            player.inventory.selected = 0;
            use_cold_remedy(game, slot);
            player.inventory.selected = 1;
            use_cold_remedy(game, slot);
        }
    } else player.inventory.selected = poultice ? 4 : 3;
}
