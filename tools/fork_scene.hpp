#pragma once
#include "../src/items/tuning_fork.hpp"
#include "../src/props/interaction.hpp"
#include "../src/particles/templates.hpp"

inline void arrange_fork_scene(Game& game,Cosmetics& cosmetics,Entity& player,bool pulse) {
    for (Entity& actor : game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; }
    for (int y=8;y<=16;++y) for (int x=12;x<=28;++x)
        *game.stage.at({x,y})={TileKind::Ruin,0,0};
    player.cell={17,12};player.facing={1,0};player.inventory={};player.scorch_ticks=0;
    insert_item(player.inventory,make_item(ItemKind::TuningFork));
    insert_item(player.inventory,make_item(ItemKind::TuningFork,1,ItemAttribute::Durable));
    insert_item(player.inventory,make_item(ItemKind::TuningFork,1,ItemAttribute::Long));
    for (int x=18;x<=22;++x) place_prop(game.stage,{x,12},PropKind::CrystalGrowth);
    for (int x=18;x<=20;++x) place_prop(game.stage,{x,9},PropKind::CrystalGrowth);
    spawn_entity(game,EntityKind::ShardColony,{23,12});
    if (pulse) ring_tuning_fork(game,game.players[0].slot,{1,0});
    for (int i=0;i<game.sound_count;++i)
        spawn_sound_effect(cosmetics,game.sounds[static_cast<std::size_t>(i)],static_cast<std::uint64_t>(800+i));
    cosmetics.camera={20,12};cosmetics.camera_ready=true;
}
