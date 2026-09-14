#pragma once
#include "../src/entities/whiteout_drummer.hpp"
#include "../src/items/snow_globe.hpp"
#include "../src/surfaces/whiteout.hpp"
#include "../src/props/interaction.hpp"

// DISPLAY: Three committed beats, a finished squall and the tool's comparison panel.
inline void arrange_whiteout_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool haze) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (Tile& tile : game.stage.tiles) { tile.prop = {}; tile.surface = {}; }
    for (int y=8;y<=21;++y)
        for (int x=10;x<=30;++x)
            *game.stage.at({x,y}) = {y<11 ? TileKind::Snow : TileKind::Ruin,0,0};
    player.cell = {20,17}; player.facing = {1,0};
    player.scorch_ticks = player.burn_ticks = 0;
    player.light = {16,1700,{212,226,223}};
    player.inventory = {};
    insert_item(player.inventory,make_item(ItemKind::SnowGlobe,2));
    insert_item(player.inventory,make_item(ItemKind::SnowGlobe,2,ItemAttribute::Big));
    for (int i=0;i<3;++i) {
        Entity* drummer = get_entity(game,spawn_entity(game,EntityKind::WhiteoutDrummer,{15+i*5,12}));
        if (!drummer) continue;
        drummer->label_a = DrummerBeats; drummer->counter_a = i+1;
        drummer->timer_a = 12; drummer->point_a = drummer->cell;
        drummer->point_b = drummer->cell+Cell{0,3};
        drummer->sprite = i==1 ? Sprite::DrummerRight : Sprite::DrummerLeft;
    }
    if (haze) raise_whiteout(game,{21,17},2,300);
    place_prop(game.stage,{15,17},PropKind::WeatherVane);
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_material(cosmetics.debris,{24,18},DebrisKind::GlobeGlass,4,5551,true);
    scatter_material(cosmetics.debris,{15,19},DebrisKind::CopperCurl,3,5553,true);
    game.tick = 60;
}
