#pragma once
#include "../src/items/sled.hpp"

inline void arrange_sled_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (Entity& actor : game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; tile.current=0; }
    for (int y=8;y<=16;++y) for (int x=12;x<=28;++x) *game.stage.at({x,y})={TileKind::Snow,0,0};
    for (int x=16;x<27;++x) *game.stage.at({x,12})={TileKind::Ice,0,0};
    player.cell={19,12};player.facing={1,0};player.inventory={};player.scorch_ticks=0;
    insert_item(player.inventory,make_item(ItemKind::Fist));
    insert_item(player.inventory,make_item(ItemKind::Sled,1,ItemAttribute::Durable));
    auto cargo=spawn_entity(game,EntityKind::GroundItem,{20,12});
    get_entity(game,cargo)->ground_item=make_item(ItemKind::Ammo);get_entity(game,cargo)->sprite=Sprite::Ammo;
    place_sled(game,player_state(game,0).controlled.slot,{1,0},make_item(ItemKind::Sled));
    player.cell={20,12};board_sled(game,player_state(game,0).controlled.slot);
    const Cell before=player.cell;player.cell={18,10};place_sled(game,player_state(game,0).controlled.slot,{0,-1},make_item(ItemKind::Sled));
    player.cell={23,13};place_sled(game,player_state(game,0).controlled.slot,{-1,0},make_item(ItemKind::Sled));player.cell=before;
    cosmetics.camera={20,12};cosmetics.camera_ready=true;
}
