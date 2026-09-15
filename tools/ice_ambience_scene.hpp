#pragma once
#include "../src/ambience/system.hpp"
#include "../src/scenery/ice_render.hpp"
#include "../src/props/stove.hpp"
#include <cstdio>

inline void describe_ice_ambience(const Game& game) {
    AmbientAudio audio;
    place_ambience(audio,game,game.run.spawn);
    std::array<int,static_cast<std::size_t>(AmbientCue::Count)> counts{};
    for (const auto& source:audio.sources) ++counts[static_cast<std::size_t>(source.cue)];
    for (std::size_t i=static_cast<std::size_t>(AmbientCue::IceGroan);i<counts.size();++i)
        if (counts[i]) std::printf("%s: %d sources\n",ambient_specs[i].name,counts[i]);
}

inline void arrange_ice_ambience_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (auto& actor:game.entities) if (&actor!=&player) actor.kind=EntityKind::None;
    for (auto& tile:game.stage.tiles) tile={TileKind::Wall,100,0};
    for (int y=8;y<=21;++y)
        for (int x=9;x<=29;++x) *game.stage.at({x,y})={TileKind::Ruin,0,0};
    for (int y=8;y<=12;++y)
        for (int x=9;x<=15;++x) *game.stage.at({x,y})={TileKind::ShallowWater,0,0};
    for (int y=10;y<=11;++y)
        for (int x=10;x<=12;++x) *game.stage.at({x,y})={TileKind::Water,0,0};
    for (int x=25;x<=29;++x) *game.stage.at({x,8})={TileKind::Snow,0,0};
    place_prop(game.stage,{24,12},PropKind::Stove);
    game.stage.at({24,12})->prop.growth_ticks=3600;
    light_stove(game,{24,12});
    place_prop(game.stage,{25,18},PropKind::BeamLamp);
    place_prop(game.stage,{21,18},PropKind::LensCase);
    spawn_entity(game,EntityKind::BoilerTank,{18,12});
    player.cell={19,15};player.light={16,1600,{208,225,225}};
    game.run.spawn=player.cell;game.tick=144;
    cosmetics={};cosmetics.camera={19,14};cosmetics.camera_ready=true;
    observe_ice_scenery(cosmetics,game,player.cell);
    describe_ice_ambience(game);
}
