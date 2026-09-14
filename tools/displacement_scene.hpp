#pragma once
#include "woodland_tools_scene.hpp"
#include "../src/projectiles/hook.hpp"
#include "../src/debris/air.hpp"

// Static illustration of the tether and horn cone; no gameplay simulation is advanced.
inline void arrange_displacement(Game& game, Cosmetics& cosmetics, Entity& player) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::RopeHook, ItemKind::HuntingHorn, ItemKind::BirdSeed})
        insert_item(player.inventory, make_item(kind));
    player.inventory.selected = 1;
    const Handle bear = spawn_entity(game, EntityKind::Bear, {18, 11});
    Entity* hook = get_entity(game, spawn_entity(game, EntityKind::Projectile, {18, 11}));
    hook->label_a = static_cast<int>(ProjectileKind::Hook);
    hook->label_b = 1;
    hook->entity_a = game.players[0];
    hook->entity_b = bear;
    hook->point_a = player.cell;
    hook->sprite = Sprite::HookHead;
    hook->facing = {1, 0};
    hook->timer_a = 22; hook->timer_b = 4; hook->counter_a = 3;
    hook->attack_interval = 6;
    hook->ground_item = *player.inventory.held();
    for (Cell cell : {Cell{13, 14}, {14, 14}, {15, 13}, {15, 15}})
        scatter_material(cosmetics.debris, cell, DebrisKind::OakLeaf, 3, 837, true);
    blow_debris(cosmetics, game, {{12, 14}, {1, 0}, 3, 2, true});
}
