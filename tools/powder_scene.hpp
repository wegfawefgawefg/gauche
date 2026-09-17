#pragma once
#include "crew_scene.hpp"
#include "../src/entities/powder_monkey.hpp"

inline void arrange_powder_scene(Game& game,Cosmetics& cosmetics,bool running) {
    arrange_crew_scene(game,cosmetics,false,false);
    for (int slot=0;slot<max_entities;++slot)
        if (game.entities[static_cast<std::size_t>(slot)].kind!=EntityKind::Player)
            remove_entity(game,{slot,game.entities[static_cast<std::size_t>(slot)].generation});
    auto& player=*get_entity(game,player_state(game,0).controlled);player.cell={18,12};player.facing={1,0};
    auto monkey=spawn_entity(game,EntityKind::PowderMonkey,{20,10});
    step_powder_monkey(game,monkey.slot);
    if (running) { get_entity(game,monkey)->timer_a=0; step_powder_monkey(game,monkey.slot); }
    cosmetics.camera={20,11};
}
