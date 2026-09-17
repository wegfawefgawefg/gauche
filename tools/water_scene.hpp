#pragma once

#include "../src/world/route.hpp"
#include "../src/world/water.hpp"
#include "../src/particles/water.hpp"

inline void arrange_water_scene(Game& game, Cosmetics& cosmetics) {
    game = {};
    cosmetics = {};
    game.started = true;
    game.tick = 120;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    player_state(game,0).online = true;
    FloorPlan plan;
    plan.width = 28; plan.height = 22;
    RoomPlan brook;
    brook.center = {14, 11};
    brook.half_width = 8; brook.half_height = 7;
    brook.role = RoomRole::Brook;
    plan.rooms.push_back(brook);
    carve_floor(game, plan);
    place_water_scenes(game, plan);
    Cell source{14, 6};
    for (int y = 0; y < plan.height; ++y)
        for (int x = 0; x < plan.width; ++x)
            if (game.stage.at({x, y})->kind == TileKind::Spring) source = {x, y};
    game.run.spawn = source + Cell{0, 2};
    game.run.exit = {20, 16};
    player_state(game,0).controlled = spawn_entity(game, EntityKind::Player, game.run.spawn);
    get_entity(game, player_state(game,0).controlled)->owner = 0;
    spawn_entity(game, EntityKind::Campfire, source + Cell{4, 3});
    cosmetics.camera = source + Cell{2, 2};
    cosmetics.camera_ready = true;
    spawn_water_rings(cosmetics, source, false);
    for (int i = 0; i < 15; ++i) step_particles(cosmetics);
    spawn_water_rings(cosmetics, game.run.spawn, true);
    for (int i = 0; i < 12; ++i) step_particles(cosmetics);
}
