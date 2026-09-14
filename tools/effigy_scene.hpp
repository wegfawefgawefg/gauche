#pragma once
#include "../src/entities/snow_effigy.hpp"

// DISPLAY: Snow shell and exposed frame, both at rest and with their warning face lit.
inline void arrange_effigy_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x)
            *game.stage.at({x,y}) = {y<12 || x<13 ? TileKind::Snow : TileKind::Ruin,0,0};
    const Sprite poses[]{Sprite::SnowEffigy,Sprite::EffigyStrike,Sprite::EffigyFrame,Sprite::EffigyFrameStrike};
    for (int i=0;i<4;++i) {
        Entity* effigy = get_entity(game,spawn_entity(game,EntityKind::SnowEffigy,{13+i*4,13}));
        if (!effigy) continue;
        effigy->sprite = poses[i]; effigy->label_b = 1;
        effigy->counter_b = i>=2 ? 1 : 0;
        effigy->label_a = i%2 ? EffigyStrike : EffigyIdle;
        effigy->timer_a = 18; effigy->point_a = effigy->cell;
        effigy->point_b = effigy->cell+Cell{1,0}; effigy->facing = {1,0};
    }
    player.cell = {20,17}; player.light = {16,1700,{212,226,223}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{24,14},DebrisKind::Twig,5,5691,true);
    scatter_material(cosmetics.debris,{20,14},DebrisKind::SnowClump,4,5693,true);
}
