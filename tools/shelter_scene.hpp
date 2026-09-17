#pragma once
#include "../src/items/snow_shelter.hpp"
#include "../src/projectiles/frost.hpp"

inline void arrange_shelter_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (Entity& actor : game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; tile.current=0; }
    for (int y=8;y<=16;++y) for (int x=12;x<=28;++x) *game.stage.at({x,y})={TileKind::Snow,0,0};
    player.cell={19,12};player.facing={1,0};player.inventory={};player.scorch_ticks=0;
    insert_item(player.inventory,make_item(ItemKind::SnowShelter));
    insert_item(player.inventory,make_item(ItemKind::SnowShelter,1,ItemAttribute::Durable));
    place_snow_shelter(game,player_state(game,0).controlled.slot,player.facing);
    const Handle bat=spawn_entity(game,EntityKind::FrostBat,{24,12});
    launch_frost_puff(game,bat.slot,{-1,0},6);
    for (Entity& shot : game.entities) if (shot.kind==EntityKind::Projectile) {shot.cell={22,12};shot.counter_a=4;}
    const Cell before=player.cell;player.cell={17,10};place_snow_shelter(game,player_state(game,0).controlled.slot,{0,-1});player.cell=before;
    game.stage.at({20,13})->prop.hp=12;
    cosmetics.camera={20,12};cosmetics.camera_ready=true;
}
