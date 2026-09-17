#pragma once
#include "crew_scene.hpp"
#include "../src/entities/ember.hpp"
#include "../src/projectiles/coal_spit.hpp"
#include "../src/surfaces/interaction.hpp"

inline void arrange_stoker_scene(Game& game,Cosmetics& cosmetics,bool quenched) {
    arrange_crew_scene(game,cosmetics,false,false);
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].kind!=EntityKind::Player)
            remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
    auto& player=*get_entity(game,player_state(game,0).controlled); player.cell={18,14}; player.facing={0,-1};
    const auto first=spawn_entity(game,EntityKind::Ember,{18,9});
    auto& a=*get_entity(game,first); a.label_a=StokerPack; a.timer_a=18;
    a.point_a=a.cell; a.point_b={0,1}; a.sprite=Sprite::StokerPack;
    const auto second=spawn_entity(game,EntityKind::Ember,{22,12});
    auto& b=*get_entity(game,second); b.label_a=StokerScoop; b.timer_a=30;
    b.point_a=b.cell; b.counter_b=0; b.sprite=Sprite::StokerScoop;
    const auto third=spawn_entity(game,EntityKind::Ember,{14,12});
    auto& c=*get_entity(game,third); c.counter_a=c.counter_b=0;
    c.label_a=StokerStrike; c.point_a=c.cell; c.point_b=c.cell+Cell{1,0}; c.sprite=Sprite::StokerSwing;
    if (quenched) { damp_stoker(a); game.stage.at(a.cell)->surface.liquid=LiquidKind::Water; game.stage.at(a.cell)->surface.liquid_ticks=600; }
    launch_coal_spit(game,first.slot,{0,1},!quenched);
    for (auto& shot:game.entities) if (shot.kind==EntityKind::Projectile) { shot.cell={18,11}; shot.counter_a=6; shot.timer_b=3; }
    cosmetics.camera={18,12};
}
