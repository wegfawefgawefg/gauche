#pragma once
#include "crew_scene.hpp"
#include "../src/props/interaction.hpp"
#include "../src/projectiles/projectile.hpp"

inline void arrange_grate_scene(Game& game,Cosmetics& cosmetics) {
    arrange_crew_scene(game,cosmetics,false,false);
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].kind!=EntityKind::Player)
            remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
    auto& player=*get_entity(game,game.players[0]);player.cell={17,14};player.facing={1,0};
    player.inventory={};insert_item(player.inventory,make_item(ItemKind::Bow));
    for (int y=12;y<=14;++y) place_prop(game.stage,{20,y},PropKind::Grate,1);
    for (int x=15;x<=17;++x) place_prop(game.stage,{x,10},PropKind::Grate,0);
    place_prop(game.stage,{18,11},PropKind::ScrapBin);
    place_prop(game.stage,{23,16},PropKind::OreBin);
    spawn_entity(game,EntityKind::Ember,{23,14});
    prepare_debris(cosmetics.debris,game.stage);
    int i=0;
    for (auto kind:{DebrisKind::BasaltChip,DebrisKind::OreFlake,DebrisKind::SteelWasher,DebrisKind::TinCurl}) {
        scatter_material(cosmetics.debris,{18+i,16},kind,3,9721U+static_cast<unsigned int>(i),true);++i;
    }
    cosmetics.camera={19,13};
}
