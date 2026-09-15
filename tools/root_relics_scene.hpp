#pragma once
#include "woodland_tools_scene.hpp"
#include "../src/particles/templates.hpp"
#include "../src/projectiles/projectile.hpp"

// Static display of tunneling stages and blink arrival flashes; no simulation ticks run.
inline void arrange_root_relics(Game& game, Cosmetics& cosmetics, Entity& player) {
    arrange_woodland_tools(game, cosmetics, player);
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) tile.prop = {};
    player.inventory = {};
    for (ItemKind kind : {ItemKind::Fist, ItemKind::BlinkSeed, ItemKind::RootDrill, ItemKind::Pickaxe})
        insert_item(player.inventory, make_item(kind));
    player.inventory.selected = 1;
    for (int x = 13; x <= 17; ++x) {
        Tile& tile = *game.stage.at({x, 9});
        tile = {TileKind::Wall, 100, 0, 100, BreakRule::DigRequired, 2};
        if (x < 15) { tile.kind = TileKind::Ruin; tile.hp = 0; }
        if (x == 15) tile.hp = 4;
    }
    Entity* drill = get_entity(game, spawn_entity(game, EntityKind::Projectile, {14, 9}));
    drill->label_a = static_cast<int>(ProjectileKind::Drill);
    drill->label_b = 1;
    drill->sprite = Sprite::DrillRoot;
    drill->facing = {1, 0};
    drill->counter_a = 3; drill->counter_b = 24;
    drill->timer_a = 260; drill->timer_b = 3;
    drill->attack_interval = 5;
    drill->ground_item = make_item(ItemKind::RootDrill);
    drill->light = {3, 850, {190, 214, 134}};
    for (Cell cell : {Cell{13, 12}, {17, 12}})
        spawn_sound_effect(cosmetics, {SoundId::SwapFold, cell, game.tick, 0, true},
                           static_cast<std::uint64_t>(cell.x));
    Entity* blink = get_entity(game, spawn_entity(game, EntityKind::Projectile, {15, 11}));
    blink->label_a = static_cast<int>(ProjectileKind::Blink);
    blink->sprite = Sprite::SwapSeed;
    blink->facing = {1, 0};
    blink->attack_interval = 5; blink->counter_a = 3; blink->timer_b = 2; blink->timer_a = 20;
    blink->ground_item = make_item(ItemKind::BlinkSeed);
    blink->light = {2, 250, {211, 164, 242}};
}
