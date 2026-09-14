#pragma once
#include "../src/entities/candle_keeper.hpp"
#include "../src/props/interaction.hpp"

// DISPLAY: Four complete body poses, guarded lamps and the chapel's supply cabinet.
inline void arrange_keeper_scene(Game& game, Cosmetics& cosmetics, Entity& player) {
    for (Entity& actor : game.entities)
        if (&actor != &player) actor.kind = EntityKind::None;
    for (int y=9;y<=21;++y)
        for (int x=10;x<=29;++x)
            *game.stage.at({x,y}) = {TileKind::Ruin,0,0};
    for (int i=0;i<4;++i) {
        Entity* keeper = get_entity(game,spawn_entity(game,EntityKind::CandleKeeper,{13+i*4,13}));
        if (!keeper) continue;
        keeper->facing = {1,0}; keeper->counter_a = keeper->cell.x; keeper->counter_b = keeper->cell.y;
        const Sprite poses[]{Sprite::CandleKeeper,Sprite::KeeperTend,Sprite::KeeperStrike,Sprite::KeeperDim};
        keeper->sprite = poses[i]; keeper->label_a = i==1 ? KeeperRelight : i==2 ? KeeperStrike : KeeperTend;
        keeper->timer_a = 24; keeper->point_b = keeper->cell+Cell{1,0}; keeper->label_b = 1;
        if (i==3) { keeper->timer_b = 120; keeper->light = {}; }
        place_prop(game.stage,keeper->point_b,PropKind::Candle);
        if (i==1 || i==3) game.stage.at(keeper->point_b)->prop.variant = 0;
    }
    place_prop(game.stage,{17,16},PropKind::CandleCabinet);
    player.cell = {20,17}; player.light = {14,1700,{210,226,224}};
    cosmetics = {}; cosmetics.camera = {20,15}; cosmetics.camera_ready = true;
    prepare_debris(cosmetics.debris,game.stage);
    scatter_prop_debris(cosmetics.debris,{23,16},PropKind::CandleCabinet,5771,true);
}
