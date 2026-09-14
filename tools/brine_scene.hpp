#pragma once
#include "../src/props/interaction.hpp"

// DISPLAY: A salted pool, sealed tins and a discarded lid; no simulation advance.
inline void arrange_brine_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::BrineFlask,3));
    insert_item(player.inventory,make_item(ItemKind::BrineFlask,3,ItemAttribute::Big));
    for (int y=12;y<=18;++y)
        for (int x=13;x<=26;++x) {
            Tile& tile = *game.stage.at({x,y});
            tile = {TileKind::Ruin,0,0};
            if (x>=17 && x<=22 && y>=13 && y<=16) tile.kind = TileKind::Ice;
            if (distance({x,y},{19,15})<=2) {
                tile.kind = TileKind::ShallowWater;
                tile.surface.liquid = LiquidKind::Brine;
                tile.surface.liquid_ticks = 360;
            }
        }
    place_prop(game.stage,{15,15},PropKind::FrozenLunchTin);
    place_prop(game.stage,{15,17},PropKind::FrozenLunchTin);
    game.stage.at({15,17})->prop.hp = 7;
    scatter_material(cosmetics.debris,{24,15},DebrisKind::TinLid,1,5611,true);
    scatter_material(cosmetics.debris,{24,15},DebrisKind::IceChip,3,5613,true);
    cosmetics.camera = {20,15};
}
