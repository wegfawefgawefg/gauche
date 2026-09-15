#pragma once
#include "../src/particles/system.hpp"
#include "../src/world/workfront.hpp"
#include "../src/entities/mine_crew.hpp"

inline void arrange_crew_scene(Game& game,Cosmetics& cosmetics,bool cutting,bool alarm) {
    game={}; cosmetics={}; game.started=true; game.rng=9181; game.tick=120;
    game.run.floor=first_floor(Biome::Industrial); game.run.phase=RunPhase::Playing;
    game.run.online[0]=true; game.run.spawn={17,16};
    game.stage.width=36; game.stage.height=28;
    game.stage.tiles.assign(36*28,{TileKind::Wall,100,0,100,BreakRule::Unbreakable,0});
    for (int y=4;y<24;++y) for (int x=4;x<32;++x) *game.stage.at({x,y})={TileKind::Ruin,0,0};
    FloorPlan plan; plan.width=36; plan.height=28; plan.protected_cells.assign(36*28,0);
    RoomPlan room; room.center={18,13}; room.half_width=9; room.half_height=7; room.role=RoomRole::Workfront;
    plan.rooms.push_back(room); place_workfront_terrain(game,plan); populate_workfront(game,room);
    game.players[0]=spawn_entity(game,EntityKind::Player,game.run.spawn);
    Entity& player=*get_entity(game,game.players[0]); player.owner=0; player.facing={0,-1}; player.cell={18,13};
    player.light={9,1800,{226,215,191}};
    for (Entity& actor:game.entities) {
        if (actor.kind==EntityKind::ShiftForeman) {
            actor.label_a=CrewWhistle; actor.point_a=actor.cell; actor.point_b={1,0};
            actor.sprite=Sprite::ForemanWhistle; actor.timer_a=24;
        }
        if (actor.kind!=EntityKind::Pickhand) continue;
        if (cutting) {
            actor.cell={20,10+actor.counter_b}; actor.point_a=actor.cell;
            actor.point_b=actor.cell+Cell{1,0}; actor.label_a=CrewCut;
            actor.sprite=Sprite::PickhandRaise; actor.timer_a=18;
        }
        if (alarm) { actor.entity_b=game.players[0]; actor.attack_wait=600; }
    }
    cosmetics.camera={18.0F,12.5F}; cosmetics.camera_ready=true;
}
