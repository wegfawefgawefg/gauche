#pragma once

#include "../src/game.hpp"

// Static item/status display fixture; it does not advance combat or input.
inline void arrange_remedies(Entity& player, bool statuses) {
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::HerbBag, ItemKind::Splint,
                         ItemKind::BitterRoot, ItemKind::Chili, ItemKind::FungalBread})
        insert_item(player.inventory, make_item(kind));
    player.health = 41;
    player.scorch_ticks = 0;
    player.inventory.selected = 4;
    if (statuses) {
        player.scorch_ticks = 90;
        player.freeze_ticks = 120;
        player.vitals = {12, 10, 420, 120, 180};
    }
}
