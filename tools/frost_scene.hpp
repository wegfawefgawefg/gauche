#pragma once
#include "../src/projectiles/frost.hpp"
#include "../src/projectiles/recoverable.hpp"

inline void arrange_frost_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 16; ++y)
        for (int x = 12; x <= 27; ++x)
            *game.stage.at({x, y}) = {TileKind::Ice, 0, 0};
    player.cell = {22, 12};
    player.facing = {-1, 0};
    player.scorch_ticks = 0;
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::IceNeedle, 4));
    insert_item(player.inventory, make_item(ItemKind::IceNeedle, 3, ItemAttribute::Long));
    insert_item(player.inventory, make_item(ItemKind::IceNeedle, 2, ItemAttribute::Heavy));
    // POSES: Static states only; this tool never advances a live encounter.
    for (int phase = 0; phase < 3; ++phase) {
        const Handle handle = spawn_entity(game, EntityKind::FrostBat, {18, 9 + phase * 3});
        Entity* bat = get_entity(game, handle);
        bat->facing = {1, 0};
        bat->label_a = phase;
        bat->timer_a = 18;
        bat->point_b = bat->cell;
        bat->sprite = phase == 0 ? Sprite::FrostBat : phase == 1 ? Sprite::FrostBatInhale : Sprite::FrostBatFlying;
        if (phase == 2 && launch_frost_puff(game, handle.slot, {1, 0}, 6)) {
            for (Entity& shot : game.entities)
                if (shot.kind == EntityKind::Projectile) { shot.cell = {20, 15}; shot.timer_b = 3; }
        }
    }
    launch_recoverable(game, player_state(game,0).controlled.slot, player.inventory.slots[0], {-1, 0});
    for (Entity& shot : game.entities)
        if (shot.kind == EntityKind::Projectile && shot.ground_item.kind == ItemKind::IceNeedle) {
            shot.cell = {20, 12}; shot.timer_b = 2;
        }
    cosmetics.camera = {20, 12};
    cosmetics.camera_ready = true;
}
