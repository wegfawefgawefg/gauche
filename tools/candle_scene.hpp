#pragma once
#include "../src/props/candle.hpp"
#include "../src/props/interaction.hpp"

// DISPLAY: Warm lamps, an extinguished wick, fuel and retained condition.
inline void arrange_candle_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x)
            *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    for (int x : {15,20,25}) place_prop(game.stage,{x,14},PropKind::Candle);
    game.stage.at({20,14})->prop.variant = 0;
    game.stage.at({25,14})->prop.hp = 3;
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::CandleStub,4));
    Item used = make_item(ItemKind::CandleStub);
    used.opened = true; used.loaded = 750; used.durability = 3;
    insert_item(player.inventory,used);
    insert_item(player.inventory,make_item(ItemKind::WickSpool));
    player.cell = {20,17}; player.light = {6,500,{212,226,223}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{24,15},DebrisKind::Wax,5,5725,true);
    scatter_material(cosmetics.debris,{24,15},DebrisKind::CharredWick,2,5727,true);
}
