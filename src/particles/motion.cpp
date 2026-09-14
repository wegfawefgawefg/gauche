#include "motion.hpp"

#include <algorithm>
#include <cmath>

void step_actor_motion(EntityPose& pose, const Entity& entity, bool same) {
    const bool teleport = !same || !pose.motion_ready || distance(pose.cell, entity.cell) > 4;
    if (teleport || entity.move_interval == 0 || entity.kind == EntityKind::Projectile) {
        pose.previous_position = pose.position = entity.cell;
        pose.movement_remaining = 0;
        pose.motion_ready = true;
        return;
    }
    pose.previous_position = pose.position;
    // GUIDE: Like Chickens, finish the displayed step within the actor's movement beat.
    if (pose.cell != entity.cell) pose.movement_remaining = std::clamp(entity.move_interval, 1, 12);
    if (pose.movement_remaining > 0) {
        const float remaining = static_cast<float>(pose.movement_remaining--);
        pose.position.x += (static_cast<float>(entity.cell.x) - pose.position.x) / remaining;
        pose.position.y += (static_cast<float>(entity.cell.y) - pose.position.y) / remaining;
    } else pose.position = entity.cell;
}

ViewCamera presented_position(const EntityPose& pose, float alpha) {
    alpha = std::clamp(alpha, 0.0F, 1.0F);
    return {std::lerp(pose.previous_position.x, pose.position.x, alpha),
            std::lerp(pose.previous_position.y, pose.position.y, alpha)};
}
