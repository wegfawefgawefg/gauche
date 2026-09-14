#pragma once
#include "../src/items/cold_flask.hpp"
#include "../src/surfaces/temperature.hpp"

inline void arrange_cold_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 16; ++y)
        for (int x = 12; x <= 27; ++x)
            *game.stage.at({x, y}) = {x >= 19 && x <= 24 && y >= 10 && y <= 14 ?
                TileKind::ShallowWater : TileKind::Snow, 0, 0};
    player.cell = {18, 12};
    player.facing = {1, 0};
    player.scorch_ticks = 0;
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::ColdFlask, 3));
    insert_item(player.inventory, make_item(ItemKind::ColdFlask, 2, ItemAttribute::Long));
    insert_item(player.inventory, make_item(ItemKind::ColdFlask, 2, ItemAttribute::Big));
    *game.stage.at({21, 12}) = {TileKind::IceHole, 0, 0};
    cold_flask_impact(game, make_item(ItemKind::ColdFlask, 1, ItemAttribute::Big), {21, 12});
    game.stage.at({23, 12})->freeze_ticks = 90;
    game.stage.at({21, 14})->freeze_ticks = 90;
    const Handle fire = spawn_entity(game, EntityKind::Campfire, {22, 9});
    quench_cell(game, get_entity(game, fire)->cell);
    const Handle chilled = spawn_entity(game, EntityKind::RimeSkater, {21, 11});
    apply_chill(*get_entity(game, chilled), 180);
    // CAPTURE: A flying flask and the pool's settled result, with no simulation advance.
    launch_projectile(game, game.players[0].slot, player.inventory.slots[0], {1, 0}, 5);
    for (Entity& shot : game.entities)
        if (shot.kind == EntityKind::Projectile) { shot.cell = {19, 12}; shot.counter_a = 4; }
    cosmetics.camera = {21, 12};
    cosmetics.camera_ready = true;
}
