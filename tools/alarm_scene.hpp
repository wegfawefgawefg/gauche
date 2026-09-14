#pragma once
#include "optics_scene.hpp"
#include "../src/item_attribute.hpp"

// DISPLAY: Wound, ringing, stopped and damaged clocks without stepping gameplay.
inline void arrange_alarm_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    arrange_optics_scene(game, cosmetics, player, false);
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::AlarmClock));
    insert_item(player.inventory, make_item(ItemKind::AlarmClock, 1, ItemAttribute::Durable));
    insert_item(player.inventory, make_item(ItemKind::ResinGlue));
    player.inventory.slots[1].durability = 13;
    player.inventory.selected = 1;
    constexpr int timers[]{480, 300, 0, 237};
    for (int i = 0; i < 4; ++i) {
        const Cell cell{18 + i*3, 15};
        place_prop(game.stage, cell, PropKind::AlarmClock);
        Prop& prop = game.stage.at(cell)->prop;
        prop.growth_ticks = static_cast<std::uint16_t>(timers[i]);
        if (i == 3) { prop.variant = 1; prop.hp = 13; }
    }
    scatter_prop_debris(cosmetics.debris, {22,18}, PropKind::AlarmClock, 81, true);
    cosmetics.debris.ready = true;
}
