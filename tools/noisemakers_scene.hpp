#pragma once

#include "woodland_tools_scene.hpp"
#include "../src/projectiles/projectile.hpp"

// Static cards and lit fuse; no simulation steps or creature behavior run here.
inline void arrange_noisemakers(Game& game, Cosmetics& cosmetics, Entity& player, bool big) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::Firecracker, 5, big ? ItemAttribute::Big : ItemAttribute::None));
    insert_item(player.inventory, make_item(ItemKind::HandBell));
    player.inventory.selected = 2;
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, {15, 11}));
    shot->label_a = static_cast<int>(ProjectileKind::Firecracker);
    shot->sprite = Sprite::FirecrackerLit;
    shot->ground_item = player.inventory.slots[1];
    shot->counter_a = 0; shot->attack_interval = 4;
    shot->timer_a = 45; shot->facing = {1, 0};
    shot->light = {1, 130, {255, 168, 75}};
    spawn_entity(game, EntityKind::Wolf, {17, 12});
    spawn_entity(game, EntityKind::Bunny, {16, 10});
}
