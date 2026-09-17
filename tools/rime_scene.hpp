#pragma once

#include "../src/items/ice_footing.hpp"
#include "../src/entities/rime_skater.hpp"

inline void arrange_rime_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& entity : game.entities)
        if (&entity != &player) entity.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 16; ++y)
        for (int x = 12; x <= 27; ++x)
            *game.stage.at({x, y}) = {y >= 15 ? TileKind::Snow : TileKind::Ice, 0, 0};
    player.cell = {22, 12};
    player.facing = {-1, 0};
    player.scorch_ticks = 0;
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::GritPouch));
    insert_item(player.inventory, make_item(ItemKind::GritPouch, 1, ItemAttribute::Durable));
    insert_item(player.inventory, make_item(ItemKind::GritPouch, 1, ItemAttribute::Big));
    player.inventory.slots[0].uses = 4;
    scatter_grit(game, player_state(game,0).controlled.slot, player.facing);
    player.inventory.selected = 0;
    for (int phase = 0; phase < 3; ++phase) {
        Entity* skater = get_entity(game, spawn_entity(game, EntityKind::RimeSkater, {16, phase == 2 ? 14 : 9 + phase * 3}));
        skater->facing = skater->point_b = {1, 0};
        skater->point_a = skater->cell;
        skater->label_a = phase;
        skater->timer_a = phase == 1 ? 22 : 0;
        skater->counter_a = 7;
        skater->sprite = phase == 0 ? Sprite::RimeSkater :
            phase == 1 ? Sprite::RimeSkaterPush : Sprite::RimeSkaterGlide;
    }
    spawn_entity(game, EntityKind::Campfire, {15, 11});
    cosmetics.camera = {20, 12};
    cosmetics.camera_ready = true;
}
