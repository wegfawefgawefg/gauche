#pragma once
#include "../src/entities/echo_hound.hpp"

// DISPLAY: Four body poses, with one committed bite. No movement or combat stepped.
inline void arrange_echo_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y = 8; y <= 21; ++y)
        for (int x = 9; x <= 31; ++x)
            *game.stage.at({x,y}) = {TileKind::Empty,0,0};
    player.cell = {20,18}; player.facing = {1,0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {16,1700,{212,226,223}};
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::Firecracker,3));
    insert_item(player.inventory,make_item(ItemKind::ThrowingRock,3));
    constexpr Sprite poses[]{Sprite::EchoHound,Sprite::EchoHoundTrail,Sprite::EchoHoundWarn,Sprite::EchoHoundRecover};
    for (int i=0;i<4;++i) {
        Entity* hound=get_entity(game,spawn_entity(game,EntityKind::EchoHound,{14+i*4,14}));
        hound->label_a=i; hound->sprite=poses[i]; hound->timer_a=12;
        hound->facing={i%2 == 0 ? 1 : -1,0};
        hound->point_a=hound->point_b=hound->cell+hound->facing;
        hound->counter_a=hound->cell.x; hound->counter_b=hound->cell.y;
    }
    cosmetics={}; cosmetics.camera={20,16}; cosmetics.camera_ready=true;
    game.tick=60;
}
