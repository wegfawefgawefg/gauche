#pragma once
#include "../src/entities/snow_burrower.hpp"
#include "../src/items/snow_tools.hpp"
#include "../src/projectiles/snowball.hpp"
#include "../src/props/interaction.hpp"

// DISPLAY: Body phases, a cleared fan and a prepared flight are static arrangements.
inline void arrange_snow_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool ball) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 19; ++y)
        for (int x = 11; x <= 30; ++x) *game.stage.at({x, y}) = {TileKind::Snow, 0, 0};
    player.cell = {21, 15};
    player.scorch_ticks = player.burn_ticks = 0;
    player.facing = {0, -1};
    player.light = {13, 1700, {212, 226, 223}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::SnowScoop));
    insert_item(player.inventory, make_item(ItemKind::SnowScoop, 1, ItemAttribute::Big));
    insert_item(player.inventory, make_item(ItemKind::Snowball, 8));
    insert_item(player.inventory, make_item(ItemKind::Snowball, 4, ItemAttribute::Long));
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule, 3));
    player.inventory.slots[0].loaded = 6;
    player.inventory.slots[0].durability = 42;
    player.inventory.selected = ball ? 3 : 0;
    const Sprite sprites[]{Sprite::SnowMound, Sprite::SnowWarn, Sprite::SnowBurrower, Sprite::SnowDive};
    const SnowPhase phases[]{SnowBurrow, SnowWarn, SnowExposed, SnowDive};
    for (int i = 0; i < 4; ++i) {
        Entity* actor = get_entity(game, spawn_entity(game, EntityKind::SnowBurrower, {15 + i * 4, 12}));
        actor->sprite = sprites[i];
        actor->label_a = phases[i];
        actor->timer_a = 20;
        actor->point_a = actor->cell;
        actor->point_b = actor->cell + Cell{0, 1};
        actor->facing = {0, 1};
    }
    for (int x = 20; x <= 22; ++x) *game.stage.at({x, 14}) = {TileKind::Empty, 0, 0};
    place_prop(game.stage, {17, 15}, PropKind::SnowCache);
    place_prop(game.stage, {17, 17}, PropKind::SnowCache);
    game.stage.at({17, 17})->prop.broken = true;
    game.stage.at({17, 17})->prop.hp = 0;
    game.stage.at({17, 17})->kind = TileKind::Empty;
    cosmetics = {};
    cosmetics.camera = {21, 14};
    cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris, game.stage);
    for (int x = 20; x <= 22; ++x)
        scatter_material(cosmetics.debris, {x, 14}, DebrisKind::SnowClump, 4, static_cast<std::uint64_t>(x), true);
    launch_snowball(game, static_cast<int>(&player - game.entities.data()), make_item(ItemKind::Snowball), {1, 0});
    for (Entity& shot : game.entities)
        if (shot.kind == EntityKind::Projectile) { shot.cell = {24, 15}; shot.counter_a = 3; shot.timer_b = 2; }
    game.tick = 60;
}
