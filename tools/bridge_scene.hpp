#pragma once
#include "../src/items/folded_bridge.hpp"
#include "../src/props/interaction.hpp"
#include "../src/surfaces/interaction.hpp"
#include "../src/particles/templates.hpp"

inline void arrange_bridge_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool broken, bool fire) {
    for (Entity& actor : game.entities)
        if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; }
    for (int y=8;y<=16;++y)
        for (int x=12;x<=27;++x)
            *game.stage.at({x,y})={x>=20 && x<=22 ? TileKind::Water : TileKind::Ruin,0,0};
    player.cell={19,12};player.facing={1,0};player.scorch_ticks=0;player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::FoldedBridge));
    insert_item(player.inventory,make_item(ItemKind::FoldedBridge,1,ItemAttribute::Durable));
    insert_item(player.inventory,make_item(ItemKind::FoldedBridge));
    place_folded_bridge(game,player_state(game,0).controlled.slot);
    if (broken) hit_prop(game,{21,12},30,player.cell);
    if (fire) {
        ignite_surface(game,{21,12});
        spawn_flame(cosmetics,{21,12},2311,false);
    }
    for (int i=0;i<game.sound_count;++i)
        spawn_sound_effect(cosmetics,game.sounds[static_cast<std::size_t>(i)],static_cast<std::uint64_t>(300+i));
    cosmetics.camera={21,12};cosmetics.camera_ready=true;
}
