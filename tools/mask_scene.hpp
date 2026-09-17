#pragma once
#include "../src/items/effigy_mask.hpp"
#include "../src/entities/snow_effigy.hpp"

inline void arrange_mask_scene(Game& game,Cosmetics& cosmetics,Entity& player,bool active) {
    for (Entity& actor : game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; tile.current=0; }
    for (int y=8;y<=16;++y) for (int x=12;x<=28;++x) *game.stage.at({x,y})={TileKind::Snow,0,0};
    player.cell={20,12};player.facing={1,0};player.inventory={};player.scorch_ticks=0;
    insert_item(player.inventory,make_item(ItemKind::EffigyMask));
    insert_item(player.inventory,make_item(ItemKind::EffigyMask,1,ItemAttribute::Durable));
    for (Cell cell : {Cell{17,12},Cell{23,12},Cell{20,15}}) {
        auto h=spawn_entity(game,EntityKind::SnowEffigy,cell);get_entity(game,h)->label_b=1;
    }
    if (active) { Input use;use.use=true;step_effigy_mask(game,player_state(game,0).controlled.slot,use); }
    cosmetics.camera={20,12};cosmetics.camera_ready=true;
}
