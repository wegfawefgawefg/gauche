#pragma once
#include "woodland_tools_scene.hpp"
#include "../src/projectiles/net.hpp"
#include "../src/surfaces/interaction.hpp"

// Static wide cloth flight, a netted creature, oil and two player status indicators.
inline void arrange_movement_tools(Game& game, Cosmetics& cosmetics, Entity& player, bool world, bool big) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::ThrowingNet, ItemKind::StickyBoots, ItemKind::RabbitCharm})
        insert_item(player.inventory, make_item(kind));
    player.inventory.slots[1] = make_item(ItemKind::ThrowingNet, 3, big ? ItemAttribute::Big : ItemAttribute::None);
    player.inventory.selected = 2;
    player.vitals.grip = world ? 240 : 0;
    player.vitals.rooted = world ? 75 : 0;
    player.vitals.root_kind = RootKind::Net;
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, {15, 10}));
    shot->sprite = Sprite::NetFlight;
    shot->label_a = static_cast<int>(ProjectileKind::Net);
    shot->label_b = big ? 31 : 7;
    shot->ground_item = player.inventory.slots[1];
    shot->facing = {1, 0};
    shot->counter_a = 2; shot->attack_interval = 3;
    shot->timer_a = 12; shot->timer_b = 2;
    shot->entity_a = game.players[0];
    Entity* wolf = get_entity(game, spawn_entity(game, EntityKind::Wolf, {16, 12}));
    wolf->vitals.rooted = 110; wolf->vitals.root_kind = RootKind::Net;
    for (Cell cell : {Cell{12, 10}, {12, 11}, {12, 12}})
        pour_surface(game, cell, LiquidKind::Oil, 600);
}
