#pragma once
#include "../src/items/flare.hpp"
#include "../src/projectiles/projectile.hpp"

inline void arrange_flare_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x) *game.stage.at({x,y})={x<17 ? TileKind::Ice : TileKind::Ruin,0,0};
    player.cell={18,17};player.facing={1,0};player.light={5,800,{206,221,224}};
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::SignalFlare,3));
    insert_item(player.inventory,make_item(ItemKind::SignalFlare,3,ItemAttribute::Long));
    insert_item(player.inventory,make_item(ItemKind::WaterFlask,3));
    auto* flare=get_entity(game,spawn_entity(game,EntityKind::Projectile,{23,14}));
    flare->label_a=static_cast<int>(ProjectileKind::Flare);flare->label_b=1;flare->timer_a=600;
    flare->ground_item=make_item(ItemKind::SignalFlare);flare->facing={1,0};
    flare->sprite=Sprite::FlareBurning;flare->light={7,1350,{255,69,42}};
    auto* flying=get_entity(game,spawn_entity(game,EntityKind::Projectile,{18,12}));
    flying->label_a=static_cast<int>(ProjectileKind::Flare);flying->counter_a=5;flying->attack_interval=8;
    flying->timer_a=22;flying->timer_b=2;flying->facing={1,0};flying->ground_item=make_item(ItemKind::SignalFlare);
    flying->sprite=Sprite::SignalFlare;flying->light={2,400,{255,91,54}};
    spawn_entity(game,EntityKind::SteamLeech,{25,15});
    cosmetics={};cosmetics.camera={20,15};cosmetics.camera_ready=true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{19,18},DebrisKind::FlareCinder,4,7301,true);
}
