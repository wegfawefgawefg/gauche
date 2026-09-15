#pragma once
#include "../src/items/heat_siphon.hpp"
#include "../src/particles/templates.hpp"
#include "../src/props/candle.hpp"
#include "../src/surfaces/interaction.hpp"

inline void arrange_siphon_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool fire) {
    for (Entity& actor : game.entities)
        if (&actor!=&player) actor.kind=EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop={}; tile.surface={}; }
    for (int y=8; y<=16; ++y)
        for (int x=12; x<=27; ++x)
            *game.stage.at({x,y})={TileKind::Ruin,0,0};
    player.cell={21,12}; player.facing={1,0}; player.scorch_ticks=0;
    player.inventory={};
    insert_item(player.inventory,make_item(ItemKind::HeatSiphon));
    insert_item(player.inventory,make_item(ItemKind::HeatSiphon,1,ItemAttribute::Big));
    insert_item(player.inventory,make_item(ItemKind::HeatSiphon,1,ItemAttribute::Strong));
    for (Item& item : player.inventory.slots)
        if (item.kind==ItemKind::HeatSiphon) item.loaded=1500;
    place_candle(game,{20,11},make_item(ItemKind::CandleStub));
    pour_surface(game,{23,12},LiquidKind::Oil,900);
    if (fire) {
        discharge_siphon(game,game.players[0].slot);
        for (int i=0;i<game.sound_count;++i)
            spawn_sound_effect(cosmetics,game.sounds[static_cast<std::size_t>(i)],static_cast<std::uint64_t>(300+i));
    }
    cosmetics.camera={21,12}; cosmetics.camera_ready=true;
}
