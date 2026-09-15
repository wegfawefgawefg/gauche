#pragma once
#include "../src/items/circuits.hpp"
#include "../src/props/circuits.hpp"
#include "../src/particles/electricity.hpp"

inline void arrange_circuit_scene(Game& game,Cosmetics& cosmetics,Entity& player) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x) *game.stage.at({x,y})={TileKind::Ruin,0,0};
    player.cell={18,17}; player.facing={1,0}; player.light={7,1100,{209,221,223}};
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::CopperWire));
    insert_item(player.inventory,make_item(ItemKind::GroundingSpike,3));
    insert_item(player.inventory,make_item(ItemKind::EelBattery));
    insert_item(player.inventory,make_item(ItemKind::CopperWire,1,ItemAttribute::Durable));
    for (int y=12;y<=16;++y)
        for (int x=13;x<=15;++x) *game.stage.at({x,y})={TileKind::ShallowWater,0,0};
    for (Cell cell : {Cell{16,14},{17,14},{18,14},{18,13},{19,13},{20,13}})
        place_prop(game.stage,cell,PropKind::CopperWire);
    place_prop(game.stage,{21,13},PropKind::GroundingSpike);
    place_prop(game.stage,{22,17},PropKind::GroundingSpike);
    game.stage.at({22,17})->prop.variant=1; game.stage.at({22,17})->prop.growth_ticks=137;
    place_prop(game.stage,{24,17},PropKind::GroundingSpike);
    game.stage.at({24,17})->prop.variant=2;
    spawn_entity(game,EntityKind::GlassEel,{14,14});
    spawn_entity(game,EntityKind::FrozenPilgrim,{20,13});
    cosmetics={}; cosmetics.camera={19,15}; cosmetics.camera_ready=true;
    prepare_debris(cosmetics.debris,game.stage);
    spawn_water_arc(cosmetics,{21,17},{22,17},941);
    scatter_material(cosmetics.debris,{24,19},DebrisKind::CopperCurl,4,943,true);
}
