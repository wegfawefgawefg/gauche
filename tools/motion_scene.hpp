#pragma once
#include "material_scene.hpp"

inline void arrange_motion_scene(Game& game, Cosmetics& cosmetics) {
    arrange_material_scene(game, cosmetics);
    Entity& player = *get_entity(game, player_state(game,0).controlled);
    player.facing = {-1, 0};
    EntityPose& pose = cosmetics.poses[static_cast<std::size_t>(player_state(game,0).controlled.slot)];
    pose.seen = pose.camera_guide_ready = true;
    pose.kind = EntityKind::Player;
    pose.generation = player.generation;
    pose.previous_camera = {13.25F, 11};
    pose.camera_position = {13.5F, 11};
    cosmetics.frame_alpha = .5F;
    constexpr Cell directions[]{Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int i = 0; i < 4; ++i) {
        Entity* model = get_entity(game, spawn_entity(game, EntityKind::Zombie, {10 + i * 2, 13}));
        model->facing = directions[i];
        model->inventory = {};
        insert_item(model->inventory, make_item(ItemKind::Pickaxe));
        model->light = {3, 650, {230, 239, 221}};
    }
}
