#pragma once
#include "../src/items/storm_lantern.hpp"

inline void arrange_lantern_scene(Game& game,Cosmetics& cosmetics,Entity& player,bool focused,bool closed) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=8;y<=23;++y)
        for (int x=10;x<=35;++x) *game.stage.at({x,y})={TileKind::Ruin,0,0};
    player.cell={17,16}; player.facing={1,0}; player.light={};
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::StormLantern));
    insert_item(player.inventory,make_item(ItemKind::StormLantern));
    player.inventory.slots[0].light.shape=focused ? LightShape::Beam : LightShape::Cone;
    player.inventory.slots[0].opened=!closed;
    player.inventory.slots[1].loaded=2317; player.inventory.slots[1].opened=false;
    for (int y=13;y<16;++y) *game.stage.at({23,y})={TileKind::Wall,100,0};
    place_prop(game.stage,{23,18},PropKind::Crate);
    spawn_entity(game,EntityKind::FrozenPilgrim,{25,16});
    cosmetics={}; cosmetics.camera={22,16}; cosmetics.camera_ready=true;
}
