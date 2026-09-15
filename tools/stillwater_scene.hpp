#pragma once
#include "../src/items/stillwater_bell.hpp"
#include "../src/world/ground_items.hpp"
#include "../src/world/floating_items.hpp"
#include "../src/particles/templates.hpp"

inline void arrange_stillwater_scene(Game& game,Cosmetics& cosmetics,Entity& player,bool calm) {
    for (Entity& actor : game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; tile.current=0; }
    for (int y=7;y<=17;++y) for (int x=12;x<=28;++x) {
        Tile& tile=*game.stage.at({x,y});
        tile={x>=19 && x<=21 ? TileKind::ShallowWater : TileKind::Ice,0,0};
        if (tile.kind==TileKind::ShallowWater) tile.current=2;
    }
    player.cell={18,12};player.facing={1,0};player.inventory={};player.scorch_ticks=0;
    insert_item(player.inventory,make_item(ItemKind::StillwaterBell));
    insert_item(player.inventory,make_item(ItemKind::StillwaterBell,1,ItemAttribute::Big));
    insert_item(player.inventory,make_item(ItemKind::StillwaterBell,1,ItemAttribute::Durable));
    auto cargo=spawn_entity(game,EntityKind::GroundItem,{20,10});
    auto& item=*get_entity(game,cargo);item.ground_item=make_item(ItemKind::SmokedFish);item.sprite=Sprite::SmokedFish;
    start_item_float(game,cargo.slot,{0,1});
    place_ground_item(game,{20,14},ItemKind::Chisel);
    if (calm) use_held_item(game,game.players[0].slot,player.cell);
    for (int i=0;i<game.sound_count;++i)
        spawn_sound_effect(cosmetics,game.sounds[static_cast<std::size_t>(i)],static_cast<std::uint64_t>(1200+i));
    cosmetics.camera={20,12};cosmetics.camera_ready=true;
}
