#pragma once
#include "../src/items/echo_pebble.hpp"
#include "../src/projectiles/projectile.hpp"

inline void arrange_echo_pebble_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=8;y<23;++y)
        for (int x=8;x<33;++x) *game.stage.at({x,y})={TileKind::Ruin,0,0};
    player.cell={16,16}; player.facing={1,0}; player.inventory={};
    Item recorded=make_item(ItemKind::EchoPebble,2);
    recorded.loaded=static_cast<int>(SoundId::BowRelease)+1; recorded.spare=7;
    insert_item(player.inventory,recorded);
    insert_item(player.inventory,make_item(ItemKind::EchoPebble,2));
    insert_item(player.inventory,make_item(ItemKind::Bow));
    Entity* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,{23,16}));
    shot->label_a=static_cast<int>(ProjectileKind::EchoPebble); shot->label_b=1;
    shot->ground_item=recorded; shot->ground_item.count=1;
    shot->counter_b=1; shot->timer_a=54; shot->use_flash=6;
    shot->sprite=Sprite::EchoPebbleReady; shot->light={2,240,{139,198,211}};
    for (Cell cell:{Cell{26,14},Cell{27,18}}) spawn_entity(game,EntityKind::EchoHound,cell);
    cosmetics={}; cosmetics.camera={21,15}; cosmetics.camera_ready=true;
}
