#pragma once
#include "../src/entities/steam_leech.hpp"
#include "../src/surfaces/temperature.hpp"
#include "../src/items/heat_capsule.hpp"

inline void arrange_leech_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool released) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 17; ++y)
        for (int x = 13; x <= 28; ++x) *game.stage.at({x, y}) = {TileKind::Ruin, 0, 0};
    player.cell = {16, 14};
    player.scorch_ticks = 0;
    player.light = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::ColdFlask, 3));
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule, 3));
    for (int index = 0; index < 3; ++index) {
        const Cell cell{17 + index * 4, 12};
        const Handle source = spawn_entity(game, EntityKind::Campfire, cell + Cell{0, -1});
        Entity* fire = get_entity(game, source);
        fire->light.radius = 4;
        Entity* leech = get_entity(game, spawn_entity(game, EntityKind::SteamLeech, cell));
        leech->entity_a = source;
        leech->point_a = cell;
        leech->point_b = fire->cell;
        leech->facing = {0, -1};
        leech->label_a = index == 0 ? LeechLatch : index == 1 ? LeechFeed : LeechSwell;
        leech->timer_a = index == 0 ? 12 : index == 1 ? 70 : 20;
        leech->sprite = index == 0 ? Sprite::LeechLatch : index == 1 ? Sprite::LeechFeed : Sprite::LeechSwell;
        if (released && index > 0) apply_chill(*leech, 120);
    }
    warm_surface(game, {21, 16}, 240);
    Entity* patch_leech = get_entity(game, spawn_entity(game, EntityKind::SteamLeech, {21, 15}));
    patch_leech->label_a = LeechFeed;
    patch_leech->label_b = 1;
    patch_leech->timer_a = 80;
    patch_leech->point_a = patch_leech->cell;
    patch_leech->point_b = {21, 16};
    patch_leech->facing = {0, 1};
    patch_leech->sprite = Sprite::LeechFeed;
    if (released) apply_chill(*patch_leech, 120);
    game.tick = 42;
    cosmetics.camera = {21, 13};
    cosmetics.camera_ready = true;
}
