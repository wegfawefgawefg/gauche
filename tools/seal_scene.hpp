#pragma once
#include "../src/entities/seal_thief.hpp"
#include "../src/props/interaction.hpp"

// DISPLAY: Actual carried loot, fixed bite pose, intact and broken fishing baskets.
inline void arrange_seal_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y=8;y<=21;++y)
        for (int x=10;x<=30;++x)
            *game.stage.at({x,y}) = {y<=12 ? TileKind::ShallowWater : TileKind::Empty,0,0};
    player.cell = {20,18}; player.facing = {1,0};
    player.burn_ticks = player.scorch_ticks = 0;
    player.light = {16,1700,{212,226,223}};
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::SmokedFish,3));
    constexpr Sprite poses[]{Sprite::SealThief,Sprite::SealCarry,Sprite::SealEat,Sprite::SealBark,Sprite::SealRest};
    constexpr int phases[]{SealForage,SealRetreat,SealEat,SealBark,SealRecover};
    for (int i=0;i<5;++i) {
        Entity* seal = get_entity(game,spawn_entity(game,EntityKind::SealThief,{14+i*3,13}));
        if (!seal) continue;
        seal->sprite = poses[i]; seal->label_a = phases[i]; seal->timer_a = 90;
        seal->facing = {1,0}; seal->point_a = seal->cell; seal->point_b = seal->cell+seal->facing;
        if (i==1 || i==2) *seal->inventory.held() = make_item(ItemKind::SmokedFish);
    }
    place_prop(game.stage,{16,17},PropKind::FishingCreel);
    place_prop(game.stage,{24,17},PropKind::FishingCreel);
    game.stage.at({24,17})->prop.broken = true;
    game.stage.at({24,17})->prop.hp = 0;
    cosmetics.debris = {};
    prepare_debris(cosmetics.debris,game.stage);
    cosmetics.camera.x = 20; cosmetics.camera.y = 15;
    cosmetics.camera_ready = true;
    game.tick = 60;
}
