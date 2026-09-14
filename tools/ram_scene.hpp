#pragma once
#include "../src/entities/avalanche_ram.hpp"

// DISPLAY: Idle, pawing, lowered horns and stunned recovery on a snowy cliff shelf.
inline void arrange_ram_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x)
            *game.stage.at({x,y}) = {y<14 || x<13 ? TileKind::Snow : TileKind::Empty,0,0};
    const Sprite poses[]{Sprite::AvalancheRam,Sprite::RamPaw,Sprite::RamLunge,Sprite::RamStagger};
    for (int i=0;i<4;++i) {
        Entity* ram = get_entity(game,spawn_entity(game,EntityKind::AvalancheRam,{13+i*4,13}));
        if (!ram) continue;
        ram->sprite = poses[i]; ram->label_a = i==3 ? RamStagger : i;
        ram->timer_a = 24; ram->counter_a = 2;
        ram->point_a = ram->cell; ram->point_b = {1,0}; ram->facing = {1,0};
        if (i==3) ram->stun_ticks = 50;
    }
    player.cell = {20,17}; player.light = {16,1700,{212,226,223}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{22,14},DebrisKind::SnowClump,4,5651,true);
}
