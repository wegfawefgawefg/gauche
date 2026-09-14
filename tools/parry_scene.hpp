#pragma once

#include "shield_scene.hpp"
#include "../src/projectiles/projectile.hpp"
#include "../src/particles/templates.hpp"

// STATIC: Raised-pan facings and representative reflected flights, without stepping combat.
inline void arrange_parries(Game& game, Cosmetics& cosmetics, Entity& player, bool world) {
    arrange_shields(game, cosmetics, player, false);
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::Fist));
    insert_item(player.inventory, make_item(ItemKind::ReflectingPan));
    insert_item(player.inventory, make_item(ItemKind::ReflectingPan, 1, ItemAttribute::Durable));
    insert_item(player.inventory, make_item(ItemKind::ResinGlue));
    player.inventory.selected = player.guard_slot = 1;
    player.inventory.slots[1].durability = 16;
    player.inventory.slots[1].cooldown = 32;
    player.block_ticks = 6;
    if (!world) return;
    game.run.roof_light_count = 0;
    const Cell cells[]{{13, 11}, {19, 11}, {13, 16}, {19, 16}};
    const Cell facings[]{{1, 0}, {-1, 0}, {0, -1}, {0, 1}};
    const Sprite sprites[]{Sprite::Arrow, Sprite::Rocket, Sprite::ThrowingRock, Sprite::Boomerang};
    const ProjectileKind kinds[]{ProjectileKind::Arrow, ProjectileKind::Rocket, ProjectileKind::Rock, ProjectileKind::Boomerang};
    for (int i = 0; i < 4; ++i) {
        Entity* guard = i == 0 ? &player : get_entity(game, spawn_entity(game, EntityKind::Player, cells[i]));
        guard->inventory = player.inventory;
        guard->cell = cells[i]; guard->facing = facings[i];
        guard->block_ticks = 6; guard->guard_slot = 1;
        Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, cells[i] + facings[i]));
        shot->sprite = sprites[i]; shot->label_a = static_cast<int>(kinds[i]);
        shot->facing = facings[i]; shot->counter_a = 5; shot->timer_b = 1;
        shot->timer_a = 25;
        if (i == 1) shot->light = {2, 380, {255, 162, 73}};
        spawn_sound_effect(cosmetics, {SoundId::PanReflect, cells[i]}, static_cast<std::uint64_t>(i + 15));
    }
    cosmetics.camera = {16, 13.5F};
}
