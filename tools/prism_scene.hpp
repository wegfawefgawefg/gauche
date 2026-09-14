#pragma once
#include "optics_scene.hpp"
#include "../src/projectiles/prism.hpp"

// DISPLAY: Fixed fuse/flight poses and a pure four-ray trace, with no combat step.
inline void arrange_prism_scene(Game& game, Cosmetics& cosmetics, Entity& player, bool burst) {
    arrange_optics_scene(game, cosmetics, player, false);
    player.cell = {17, 19};
    game.stage.at(player.cell)->prop = {};
    player.inventory = {};
    insert_item(player.inventory, make_item(ItemKind::PrismBomb, 2));
    insert_item(player.inventory, make_item(ItemKind::PrismBomb, 2, ItemAttribute::Big));
    insert_item(player.inventory, make_item(ItemKind::PrismBomb, 2, ItemAttribute::Long));
    player.inventory.selected = 0;
    const Cell center{19, 16};
    if (burst) {
        const BeamTrace trace = trace_beam_burst(game, center, 16, 4, false);
        for (int i = 0; i < trace.count; ++i) {
            const BeamCell& hit = trace.cells[static_cast<std::size_t>(i)];
            game.shots[static_cast<std::size_t>(game.shot_count++)] = {hit.from, hit.cell, hit.stop, i == 0, false, false, true};
        }
        observe_gunfire(cosmetics, game, center);
        scatter_material(cosmetics.debris, center, DebrisKind::CrystalSplinter, 6, 12491);
        return;
    }
    const Cell cells[]{{16, 14}, center, {23, 18}};
    for (int i=0; i<3; ++i) {
        Entity* bomb = get_entity(game, spawn_entity(game, EntityKind::Projectile, cells[i]));
        bomb->label_a = static_cast<int>(ProjectileKind::PrismBomb);
        bomb->sprite = i == 2 ? Sprite::PrismBombReady : Sprite::PrismBombLit;
        bomb->light = {2, i == 2 ? 300 : 160, {184, 209, 255}};
        bomb->timer_a = i == 0 ? 80 : i == 1 ? 45 : 15;
        bomb->counter_a = i == 0 ? 2 : 0;
        bomb->attack_interval = 3;
        bomb->timer_b = 4;
        bomb->facing = {1, 0};
        bomb->ground_item = make_item(ItemKind::PrismBomb);
    }
}
