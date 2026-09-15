#pragma once
#include "crew_scene.hpp"
#include "../src/entities/strikebreaker.hpp"

inline void arrange_breaker_scene(Game& game,Cosmetics& cosmetics,bool raised) {
    arrange_crew_scene(game,cosmetics,false,false);
    for (int i=0;i<3;++i) {
        auto* guard=get_entity(game,spawn_entity(game,EntityKind::Strikebreaker,{16+i*3,15}));
        guard->facing=i==0 ? Cell{-1,0} : i==1 ? Cell{0,1} : Cell{1,0};
        guard->point_a=guard->cell; guard->point_b=guard->cell+guard->facing;
        if (raised && i==1) { guard->label_a=BreakerHammer; guard->timer_a=30; guard->sprite=Sprite::BreakerRaise; }
        if (i==2) { guard->counter_c=0; guard->sprite=Sprite::BreakerUnshielded; }
    }
    auto& player=*get_entity(game,game.players[0]);player.cell={18,17};
    cosmetics.camera={18,13};
}
