#pragma once

#include "../src/world/ice_terrain.hpp"
#include "../src/props/interaction.hpp"
#include "../src/particles/water.hpp"

inline void arrange_ice_scene(Game& game, Cosmetics& cosmetics, bool generated,
                               std::uint64_t seed) {
    game = {};
    cosmetics = {};
    game.rng = seed == 0 ? 1 : seed;
    game.run.seed = game.rng;
    game.run.floor = first_floor(Biome::Ice);
    game.run.phase = RunPhase::Playing;
    game.run.online[0] = true;
    if (generated) {
        // PARTY: Enter through a real initial loadout, then carry it to the cold floor.
        start_run(game, seed == 0 ? 1 : seed);
        game.run.floor = first_floor(Biome::Ice);
        generate_world_floor(game);
        return;
    }
    game.started = true;
    game.tick = 120;
    FloorPlan plan;
    plan.width = 38; plan.height = 28;
    RoomPlan room;
    room.center = {19, 14};
    room.half_width = 9; room.half_height = 8;
    room.role = RoomRole::Reservoir;
    plan.rooms.push_back(room);
    carve_floor(game, plan);
    place_ice_terrain(game, plan);
    game.run.spawn = {19, 13};
    game.players[0] = spawn_entity(game, EntityKind::Player, game.run.spawn);
    spawn_entity(game, EntityKind::Campfire, {14, 12});
    spawn_entity(game, EntityKind::FrostBat, {24, 17});
    place_prop(game.stage, {12, 12}, PropKind::Crate);
    for (int x = 16; x < 23; ++x)
        spawn_footprint(cosmetics, {x, 14}, EntityKind::Player, x % 2 == 0,
                        static_cast<std::uint64_t>(x));
    spawn_water_rings(cosmetics, {15, 12}, false);
    cosmetics.camera = {19.5F, 14.0F};
    cosmetics.camera_ready = true;
}
