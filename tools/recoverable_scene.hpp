#pragma once
#include "woodland_tools_scene.hpp"
#include "../src/projectiles/projectile.hpp"

// Static item reservation and flight poses; no gameplay ticks run.
inline void arrange_recoverables(Game& game, Cosmetics& cosmetics, Entity& player) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::ThrowingRock, ItemKind::Boomerang})
        insert_item(player.inventory, make_item(kind));
    player.inventory.selected = 2;
    for (int index = 0; index < 2; ++index) {
        const Handle handle = spawn_entity(game, EntityKind::Projectile, {15, 10 + index * 2});
        Entity& shot = *get_entity(game, handle);
        shot.label_a = static_cast<int>(index == 0 ? ProjectileKind::Boomerang : ProjectileKind::Rock);
        shot.ground_item = make_item(index == 0 ? ItemKind::Boomerang : ItemKind::ThrowingRock);
        shot.sprite = item_sprite(shot.ground_item);
        shot.entity_a = player_state(game,0).controlled;
        shot.counter_a = 3; shot.attack_interval = 6;
        shot.timer_a = 40; shot.timer_b = 1;
        shot.facing = {1, 0};
        if (index == 0) player.inventory.held()->flight = handle;
    }
}
