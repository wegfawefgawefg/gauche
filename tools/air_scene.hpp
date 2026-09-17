#pragma once
#include "../src/world/floating_items.hpp"
#include "../src/world/ground_items.hpp"

inline void arrange_air_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 16; ++y)
        for (int x = 12; x <= 27; ++x)
            *game.stage.at({x, y}) = {y == 12 && x >= 19 && x <= 25 ? TileKind::ShallowWater : TileKind::Snow, 0, 0};
    player.cell = {18, 12};
    player.facing = {1, 0};
    player.scorch_ticks = 0;
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::AirBladder));
    insert_item(player.inventory, make_item(ItemKind::AirBladder, 1, ItemAttribute::Durable));
    insert_item(player.inventory, make_item(ItemKind::AirBladder, 1, ItemAttribute::Big));
    const Handle handle = spawn_entity(game, EntityKind::GroundItem, {19, 12});
    Entity* cargo = get_entity(game, handle);
    cargo->ground_item = make_item(ItemKind::Pickaxe, 1, ItemAttribute::Heavy);
    cargo->ground_item.durability = 17;
    cargo->sprite = item_sprite(cargo->ground_item);
    use_held_item(game, player_state(game,0).controlled.slot, {19, 12});
    // CAPTURE: Show a mid-trip state without advancing gameplay or moving the camera.
    cargo->cell = cargo->point_a = {21, 12};
    cargo->counter_a = item_float_reach - 2;
    cargo->timer_a = 6;
    place_ground_item(game, {25, 10}, ItemKind::AirBladder);
    cosmetics.camera = {21, 12};
    cosmetics.camera_ready = true;
}
