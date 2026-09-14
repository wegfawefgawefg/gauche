#pragma once
#include "../src/items/quarry_tools.hpp"
#include "../src/projectiles/ice_brick.hpp"
#include "../src/props/ice_cover.hpp"

// DISPLAY: Prepared action/flight state is arranged directly; no simulated playthrough.
inline void arrange_quarry_tools(Game& game, Cosmetics& cosmetics, Entity& player,
    bool brick, bool preparing) {
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Chisel));
    insert_item(player.inventory, make_item(ItemKind::Chisel, 1, ItemAttribute::Fragile));
    insert_item(player.inventory, make_item(ItemKind::IceBrick, 3));
    insert_item(player.inventory, make_item(ItemKind::IceBrick, 3, ItemAttribute::Long));
    insert_item(player.inventory, make_item(ItemKind::ResinGlue, 2));
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule, 3));
    player.inventory.selected = brick ? 3 : 1;
    player.scorch_ticks = player.burn_ticks = 0;
    if (!preparing) return;
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y = 10; y <= 17; ++y)
        for (int x = 13; x <= 25; ++x) *game.stage.at({x, y}) = {TileKind::Snow, 0, 0};
    player.cell = {18, 14};
    player.light = {9, 1500, {205, 225, 230}};
    player.inventory.selected = 2;
    player.facing = {1, 0};
    player.label_b = 3;
    player.counter_a = brick ? brick_throw_hold_ticks : 6;
    player.ground_item = *player.inventory.held();
    player.point_b = player.facing;
    place_ice_cover(game, {17, 12});
    launch_ice_brick(game, static_cast<int>(&player - game.entities.data()), *player.inventory.held(), {1, 0});
    for (Entity& shot : game.entities)
        if (shot.kind == EntityKind::Projectile) {
            shot.cell = {21, 14};
            shot.counter_a = 2;
            shot.timer_b = 4;
        }
    cosmetics = {};
    cosmetics.camera = {20, 14};
    cosmetics.camera_ready = true;
}
