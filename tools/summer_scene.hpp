#pragma once
#include "../src/items/borrowed_summer.hpp"
#include "../src/particles/templates.hpp"
#include "../src/surfaces/interaction.hpp"
#include "../src/surfaces/render.hpp"

inline void arrange_summer_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; }
    for (int y=8; y<=16; ++y)
        for (int x=12; x<=27; ++x)
            *game.stage.at({x,y})={TileKind::Ice,0,0};
    player.cell={21,12}; player.facing={1,0}; player.scorch_ticks=0;
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::BorrowedSummer));
    insert_item(player.inventory,make_item(ItemKind::BorrowedSummer,1,ItemAttribute::Durable));
    insert_item(player.inventory,make_item(ItemKind::BorrowedSummer,1,ItemAttribute::Big));
    player.inventory.selected=2;
    pour_surface(game,{23,12},LiquidKind::Oil,900);
    use_held_item(game,player_state(game,0).controlled.slot,player.cell);
    player.inventory.selected=0;
    game.tick=36;
    observe_surfaces(cosmetics,game,player.cell);
    spawn_summer_motes(cosmetics,player.cell,2011);
    cosmetics.camera={21,12}; cosmetics.camera_ready=true;
}
