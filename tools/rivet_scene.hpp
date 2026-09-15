#pragma once
#include "grate_scene.hpp"
#include "../src/entities/rivet_gunner.hpp"
#include "../src/items/rivet_gun.hpp"

inline void arrange_rivet_scene(Game& game,Cosmetics& cosmetics) {
    arrange_grate_scene(game,cosmetics);
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].kind==EntityKind::Ember)
            remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
    auto& player=*get_entity(game,game.players[0]);player.cell={17,14};
    player.inventory={};insert_item(player.inventory,make_item(ItemKind::RivetGun));
    insert_item(player.inventory,make_item(ItemKind::RivetGun,1,ItemAttribute::Piercing));
    insert_item(player.inventory,make_item(ItemKind::Ammo));
    auto& gunner=*get_entity(game,spawn_entity(game,EntityKind::RivetGunner,{22,14}));
    gunner.label_a=GunnerBrace;gunner.timer_a=15;gunner.counter_a=3;
    gunner.point_a=gunner.cell;gunner.point_b=gunner.facing={-1,0};gunner.sprite=Sprite::GunnerBrace;
    auto& loader=*get_entity(game,spawn_entity(game,EntityKind::RivetGunner,{23,12}));
    loader.label_a=GunnerReload;loader.timer_a=50;loader.sprite=Sprite::GunnerReload;
    launch_projectile(game,game.players[0].slot,*player.inventory.held(),{1,0},8);
    for (auto& shot:game.entities) if (shot.kind==EntityKind::Projectile) { shot.cell={19,14};shot.timer_b=2; }
    scatter_material(cosmetics.debris,player.cell,DebrisKind::RivetCasing,3,9851,true);
}
