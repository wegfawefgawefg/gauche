#pragma once
#include "flier_scene.hpp"
#include "../src/entities/dispatch.hpp"
#include "../src/world/terrain_material.hpp"
#include "../src/surfaces/interaction.hpp"
#include "../src/particles/templates.hpp"

inline void arrange_woodland_scene(Game& game, Cosmetics& cosmetics) {
    arrange_flier_scene(game, cosmetics);
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind != EntityKind::None && entity.kind != EntityKind::Player)
            remove_entity(game, {slot, entity.generation});
    }
    for (int x = 9; x < 13; ++x) *game.stage.at({x, 9}) = wood_tile(TileMaterial::Timber);
    for (Cell cell : {Cell{17, 9}, {18, 9}, {18, 10}, {17, 13}, {19, 14}})
        *game.stage.at(cell) = wood_tile(TileMaterial::Tree);
    game.stage.at({11, 9})->hp = 24;
    game.stage.at({17, 9})->hp = 36;
    damage_tile(game.stage, {9, 9}, 100);
    damage_tile(game.stage, {19, 14}, 100);
    ignite_surface(game, {12, 9});
    spawn_flame(cosmetics, {12, 9}, 891, false);
    spawn_campfire_smoke(cosmetics, {12, 9}, 892);
    Entity* head = get_entity(game, spawn_burrow_worm(game, {13, 15}));
    head->label_b = 1; head->sprite = Sprite::BurrowWormBite;
    head->point_a = head->cell + head->facing; head->timer_a = 16;
    head->light = {4, 1000, {238, 211, 163}};
    game.run.roof_light_count = 1;
    game.run.roof_lights[0] = {{15, 10}, {7, 1200, {231, 219, 174}}};
}
