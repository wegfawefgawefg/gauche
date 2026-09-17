#pragma once
#include "../src/items/ice_anchor.hpp"

inline void arrange_anchor_scene(Game& game,Cosmetics& cosmetics,Entity& player,bool blocked) {
    for (Entity& actor : game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; tile.current=0; }
    for (int y=8;y<=16;++y) for (int x=12;x<=28;++x) *game.stage.at({x,y})={TileKind::Snow,0,0};
    player.cell={18,12};player.facing={1,0};player.inventory={};player.scorch_ticks=0;
    insert_item(player.inventory,make_item(ItemKind::IceAnchor));
    insert_item(player.inventory,make_item(ItemKind::IceAnchor,1,ItemAttribute::Durable));
    Input use;use.use=true;step_anchor_action(game,player_state(game,0).controlled.slot,use);
    player.inventory.held()->opened=false;player.inventory.held()->cooldown=0;
    player.cell={22,13};
    if (blocked) *game.stage.at({20,12})={TileKind::Wall,60,0};
    else step_anchor_action(game,player_state(game,0).controlled.slot,use);
    cosmetics.camera={20,12};cosmetics.camera_ready=true;
}
