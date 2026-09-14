#pragma once
#include "../src/items/kettle.hpp"
#include "../src/props/interaction.hpp"
#include "../src/particles/templates.hpp"

// DISPLAY: Compare one cold charge with a boiling one; no simulation is advanced.
inline void arrange_kettle_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x) *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    place_prop(game.stage,{17,15},PropKind::Stove);
    for (int y=12;y<=14;++y) game.stage.at({23,y})->kind = TileKind::ShallowWater;
    player.inventory = {};
    Item cold = make_item(ItemKind::SteamKettle); cold.loaded = 1;
    Item hot = cold; hot.loaded = 2; hot.spare = 1241;
    insert_item(player.inventory,cold); insert_item(player.inventory,hot);
    insert_item(player.inventory,make_item(ItemKind::SteamKettle));
    Item big = make_item(ItemKind::SteamKettle,1,ItemAttribute::Big);
    big.loaded = 2; big.spare = 1731; insert_item(player.inventory,big);
    player.cell = {20,17}; player.facing = {1,0}; player.light = {10,1250,{210,226,224}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    for (Cell cell : kettle_cells(game,hot,player.cell,player.facing))
        spawn_sound_effect(cosmetics,{SoundId::KettleScald,cell},5941+static_cast<unsigned>(cell.x*31+cell.y));
}
