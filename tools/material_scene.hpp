#pragma once

#include "../src/items/materials.hpp"
#include "../src/props/interaction.hpp"
#include "../src/surfaces/render.hpp"
#include "../src/surfaces/interaction.hpp"

inline void arrange_material_scene(Game& game, Cosmetics& cosmetics) {
    game = {};
    cosmetics = {};
    game.started = true;
    game.tick = 120;
    game.run.floor = 1;
    game.run.phase = RunPhase::Playing;
    game.run.online[0] = true;
    game.stage.width = 28; game.stage.height = 22;
    game.stage.tiles.assign(28 * 22, {TileKind::Wall, 100, 0});
    for (int y = 4; y < 18; ++y)
        for (int x = 4; x < 24; ++x)
            *game.stage.at({x, y}) = {TileKind::Grass, 0, 0};
    game.run.spawn = {14, 11};
    game.players[0] = spawn_entity(game, EntityKind::Player, game.run.spawn);
    get_entity(game, game.players[0])->owner = 0;
    cosmetics.camera = {14.0F, 11.0F};
    cosmetics.camera_ready = true;
    material_impact(game, make_item(ItemKind::OilFlask), {11, 9});
    material_impact(game, make_item(ItemKind::SapJar), {17, 9});
    material_impact(game, make_item(ItemKind::MushroomSpores), {18, 13});
    material_impact(game, make_item(ItemKind::SmokePot), {10, 14});
    pour_surface(game, {14, 9}, LiquidKind::Water, 360);
    place_prop(game.stage, {10, 8}, PropKind::TallGrass);
    for (Cell cell : {Cell{11, 9}, {10, 9}, {11, 8}, {10, 8}})
        ignite_surface(game, cell);
    for (int age = 0; age < 30; ++age) {
        step_particles(cosmetics);
        if (age % 6 == 0) observe_surfaces(cosmetics, game, game.run.spawn);
    }
}
