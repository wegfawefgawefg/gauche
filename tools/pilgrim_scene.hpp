#pragma once
#include "../src/entities/frozen_pilgrim.hpp"

// DISPLAY: Transformation and attack poses with their actual saved phase fields.
inline void arrange_pilgrim_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 21; ++y)
        for (int x = 9; x <= 31; ++x) *game.stage.at({x,y}) = {TileKind::Empty,0,0};
    player.cell = {20,18}; player.facing = {1,0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {16,1700,{212,226,223}};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::ColdFlask,3));
    insert_item(player.inventory, make_item(ItemKind::HeatCapsule,4));
    constexpr Sprite poses[]{Sprite::FrozenPilgrim, Sprite::PilgrimThawing,
        Sprite::PilgrimThawed, Sprite::PilgrimStrike, Sprite::PilgrimFreezing};
    constexpr int phases[]{PilgrimCold, PilgrimThaw, PilgrimHunt, PilgrimStrike, PilgrimFreeze};
    for (int i=0;i<5;++i) {
        Entity* pilgrim = get_entity(game,spawn_entity(game,EntityKind::FrozenPilgrim,{12+i*4,14}));
        pilgrim->label_a = phases[i]; pilgrim->label_b = i>=2 ? 1 : 0;
        pilgrim->sprite = poses[i]; pilgrim->timer_a = 12; pilgrim->timer_b = 180;
        pilgrim->move_interval = i>=2 && i<4 ? 8 : 36;
        pilgrim->facing = {i%2 == 0 ? 1 : -1,0};
        pilgrim->point_a = pilgrim->cell; pilgrim->point_b = pilgrim->cell + pilgrim->facing;
    }
    spawn_entity(game,EntityKind::Campfire,{16,13});
    cosmetics = {}; cosmetics.camera = {20,16}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{24,17},DebrisKind::WoolTuft,4,5117,true);
    scatter_material(cosmetics.debris,{16,14},DebrisKind::IceChip,3,5119,true);
    cosmetics.debris.ready = true;
    game.tick = 60;
}
