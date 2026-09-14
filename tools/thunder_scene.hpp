#pragma once

#include "woodland_tools_scene.hpp"
#include "../src/projectiles/projectile.hpp"
#include "../src/projectiles/thunder.hpp"
#include "../src/particles/electricity.hpp"

// STATIC: Show the traveling seed and the dry/wet chain without advancing combat.
inline void arrange_thunder(Game& game, Cosmetics& cosmetics, Entity& player, bool world) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::ThunderAcorn, 3));
    insert_item(player.inventory, make_item(ItemKind::ThunderAcorn, 3, ItemAttribute::Big));
    player.inventory.selected = 1;
    if (!world) return;
    const Handle first = spawn_entity(game, EntityKind::Zombie, {16, 11});
    spawn_entity(game, EntityKind::Wolf, {17, 11});
    spawn_entity(game, EntityKind::Bear, {20, 11});
    spawn_entity(game, EntityKind::Boar, {20, 12});
    game.stage.at({17, 11})->kind = TileKind::ShallowWater;
    const ThunderChain chain = thunder_chain(game, first.slot, item_pattern(*player.inventory.held()));
    Cell from{15, 11};
    for (int index = 0; index < chain.count; ++index) {
        const Cell to = chain.cells[static_cast<std::size_t>(index)];
        spawn_electric_arc(cosmetics, from, to, static_cast<std::uint64_t>(index + 594));
        from = to;
    }
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, {15, 13}));
    shot->label_a = static_cast<int>(ProjectileKind::ThunderAcorn);
    shot->sprite = Sprite::ThunderAcornLit;
    shot->facing = {1, 0}; shot->timer_b = 2; shot->timer_a = 12;
    shot->counter_a = 3; shot->attack_interval = 5;
    shot->ground_item = *player.inventory.held();
    shot->light = {2, 330, {164, 215, 255}};
}
