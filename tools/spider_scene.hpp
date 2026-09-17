#pragma once
#include "../src/entities/icicle_spider.hpp"
#include "../src/particles/templates.hpp"

// DISPLAY: Two real strands cross narrow stone passages; a third spider shows its tell.
inline void arrange_spider_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool caught) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x) *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    for (int y=10;y<=15;++y) {
        *game.stage.at({15,y}) = {TileKind::Wall,100,0};
        *game.stage.at({21,y}) = {TileKind::Wall,100,0};
    }
    player.cell = {18,17}; player.light = {10,1250,{210,226,224}};
    Entity* spider = get_entity(game,spawn_entity(game,EntityKind::IcicleSpider,{16,13}));
    spider->point_a = {15,13}; spider->point_b = {21,13};
    weave_spider_strand(game,*spider); spider->label_a = SpiderWait;
    *game.stage.at({26,11}) = {TileKind::Wall,100,0};
    *game.stage.at({26,17}) = {TileKind::Wall,100,0};
    Entity* second = get_entity(game,spawn_entity(game,EntityKind::IcicleSpider,{26,12}));
    second->point_a = {26,11}; second->point_b = {26,17};
    weave_spider_strand(game,*second); second->label_a = SpiderWait;
    Entity* working = get_entity(game,spawn_entity(game,EntityKind::IcicleSpider,{17,19}));
    working->label_a = SpiderWeave; working->timer_a = 35; working->sprite = Sprite::SpiderWeave;
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    if (caught) {
        player.cell = {18,13};
        enter_spider_strand(game,player_state(game,0).controlled.slot);
        spawn_sound_effect(cosmetics,{SoundId::SpiderCatch,player.cell},6027);
    }
}
