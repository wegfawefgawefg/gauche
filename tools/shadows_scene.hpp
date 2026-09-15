#pragma once
#include "../src/debug/panels.hpp"
#include "../src/particles/templates.hpp"
#include "../src/props/interaction.hpp"
#include "../src/projectiles/projectile.hpp"

// Static comparison of contacts, raised flight, a falling survivor and loose scraps.
inline void arrange_shadows(Game& game,Cosmetics& cosmetics,Entity& player,bool enabled) {
    for (Entity& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    game.stage.width=40; game.stage.height=28; game.stage.tiles.assign(40*28,{});
    game.run.phase=RunPhase::Playing; game.run.floor=1; game.tick=120;
    player.cell={16,12}; player.scorch_ticks=player.burn_ticks=0;
    player.light={24,2200,{255,247,225}}; player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::Fist));
    cosmetics={}; cosmetics.camera={16.5F,12.5F}; cosmetics.camera_ready=true;
    debug_panels().contact_shadows=enabled;
    int x=8;
    for (EntityKind kind:{EntityKind::Chicken,EntityKind::Zombie,EntityKind::Owl,EntityKind::ZombieStack,EntityKind::Wasp}) {
        Entity* actor=get_entity(game,spawn_entity(game,kind,{x,9}));
        if (kind==EntityKind::Chicken) actor->sprite=Sprite::Chick;
        if (kind==EntityKind::Owl) { actor->sprite=Sprite::OwlFlying; actor->label_a=2; actor->point_b={x,10}; }
        if (kind==EntityKind::Zombie) { actor->label_b=1; actor->timer_b=12; actor->point_a=actor->cell; }
        if (kind==EntityKind::ZombieStack) actor->counter_a=3;
        x+=4;
    }
    x=8;
    for (PropKind kind:{PropKind::Crate,PropKind::ClayPot,PropKind::Fern,PropKind::Candle,PropKind::Nest,PropKind::Leaves}) {
        place_prop(game.stage,{x,14},kind); x+=3;
    }
    Entity* shot=get_entity(game,spawn_entity(game,EntityKind::Projectile,{19,12}));
    shot->label_a=static_cast<int>(ProjectileKind::Bomb); shot->sprite=Sprite::Bomb;
    shot->facing={1,0}; shot->counter_a=2; shot->attack_interval=4; shot->timer_b=4;
    shot->ground_item=make_item(ItemKind::Bomb);
    auto* item=get_entity(game,spawn_entity(game,EntityKind::GroundItem,{12,12}));
    item->ground_item=make_item(ItemKind::Pickaxe); item->sprite=Sprite::Pickaxe;
    for (int i=0;i<static_cast<int>(DebrisKind::Count);++i) {
        LoosePiece piece; piece.kind=static_cast<DebrisKind>(i);
        piece.x=8.5F+static_cast<float>(i%16); piece.y=16.5F+static_cast<float>(i/16);
        piece.angle=static_cast<float>((i*47)%360); piece.count=2; cosmetics.debris.pieces.push_back(piece);
    }
    for (int i=0;i<4;++i) spawn_footprint(cosmetics,{14+i,11},EntityKind::Player,i%2==0,static_cast<std::uint64_t>(i));
}
