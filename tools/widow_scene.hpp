#pragma once
#include "../src/entities/fishing_widow.hpp"
#include "../src/entities/glass_eel.hpp"
#include "../src/projectiles/widow_hook.hpp"
#include "../src/item_attribute.hpp"

// DISPLAY: Four body poses and an attached line at the actors' real cells.
inline void arrange_widow_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool fish) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y=8;y<=21;++y)
        for (int x=10;x<=30;++x)
            *game.stage.at({x,y}) = {y>=15 && y<=17 && x>=18 && x<=25 ? TileKind::ShallowWater : TileKind::Empty,0,0};
    player.cell = {21,16}; player.facing = {-1,0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.health = 64;
    player.light = {16,1700,{212,226,223}};
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::SmokedFish,3));
    insert_item(player.inventory,make_item(ItemKind::SmokedFish,2,ItemAttribute::Restorative));
    insert_item(player.inventory,make_item(ItemKind::FishingLine));
    if (!fish) {
        constexpr Sprite poses[]{Sprite::FishingWidow,Sprite::WidowWindup,Sprite::WidowUntangle};
        constexpr int phases[]{WidowHunt,WidowWindup,WidowUntangle};
        for (int i=0;i<3;++i) {
            Entity* widow = get_entity(game,spawn_entity(game,EntityKind::FishingWidow,{14+i*6,11}));
            widow->label_a = phases[i]; widow->sprite = poses[i]; widow->timer_a = 18;
            widow->facing = {1,0}; widow->point_a = widow->cell;
        }
        const Handle owner = spawn_entity(game,EntityKind::FishingWidow,{16,16});
        Entity* widow = get_entity(game,owner);
        widow->facing = {1,0}; widow->point_a = widow->cell;
        widow->entity_a = launch_widow_hook(game,owner.slot);
        widow->label_a = WidowLine; widow->sprite = Sprite::WidowReel;
        Entity* hook = get_entity(game,widow->entity_a);
        hook->cell = player.cell; hook->entity_b = game.players[0];
        hook->label_b = 1; hook->counter_a = 0; hook->timer_b = 5;
    } else {
        player.cell = {17,16};
        spawn_entity(game,EntityKind::GlassEel,{20,16});
        Entity* bait = get_entity(game,spawn_entity(game,EntityKind::GroundItem,{24,16}));
        bait->ground_item = make_item(ItemKind::SmokedFish,2); bait->sprite = Sprite::SmokedFish;
    }
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{23,18},DebrisKind::FishBone,3,5341,true);
    cosmetics.debris.ready = true;
    game.tick = 60;
}
