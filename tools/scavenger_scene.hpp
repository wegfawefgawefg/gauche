#pragma once
#include "flier_scene.hpp"

inline void arrange_scavenger_scene(Game& game, Cosmetics& cosmetics) {
    arrange_flier_scene(game, cosmetics);
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& entity = game.entities[static_cast<std::size_t>(slot)];
        if (entity.kind != EntityKind::None && entity.kind != EntityKind::Player)
            remove_entity(game, {slot, entity.generation});
    }
    const auto actor = [&](EntityKind kind, Cell cell) -> Entity& {
        Entity& entity = *get_entity(game, spawn_entity(game, kind, cell));
        entity.light = {4, 950, {242, 223, 187}};
        entity.facing = {1, 0};
        return entity;
    };
    actor(EntityKind::WaspNest, {10, 10}).counter_b = 80;
    Entity& stirring = actor(EntityKind::WaspNest, {14, 9});
    stirring.counter_b = 20; stirring.sprite = Sprite::WaspNestStirring;
    Entity& empty = actor(EntityKind::WaspNest, {18, 10});
    empty.counter_a = 0; empty.sprite = Sprite::WaspNestEmpty;
    actor(EntityKind::Wasp, {12, 11});
    Entity& sting = actor(EntityKind::Wasp, {16, 11});
    sting.label_a = 1; sting.sprite = Sprite::WaspSting;
    Entity& goblin = actor(EntityKind::ForagerGoblin, {10, 14});
    insert_item(goblin.inventory, make_item(ItemKind::Crossbow));
    actor(EntityKind::ForagerGoblin, {12, 15}).sprite = Sprite::ForagerGoblinKnife;
    Entity& crow = actor(EntityKind::CarrionCrow, {16, 15});
    insert_item(crow.inventory, make_item(ItemKind::RawMeat));
    actor(EntityKind::CarrionCrow, {18, 14}).sprite = Sprite::CarrionCrowSnatch;
}
