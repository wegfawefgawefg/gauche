#pragma once
#include "../src/props/interaction.hpp"

// DISPLAY: Lit and cold iron share a silhouette; coal carries its real thrown attributes.
inline void arrange_coal_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x)
            *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    place_prop(game.stage,{16,14},PropKind::Stove);
    place_prop(game.stage,{23,14},PropKind::Stove);
    game.stage.at({23,14})->prop.variant = 0;
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::CoalLump,8));
    insert_item(player.inventory,make_item(ItemKind::CoalLump,5,ItemAttribute::Long));
    player.cell = {20,17}; player.light = {10,1250,{210,226,224}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{16,15},DebrisKind::CoalCrumb,6,5841,true);
}
