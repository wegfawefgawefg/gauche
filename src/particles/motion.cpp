#include "motion.hpp"

#include <algorithm>
#include <cmath>

void step_camera_guide(EntityPose& pose, const Entity& entity, bool same) {
    const bool teleport = !same || !pose.camera_guide_ready || distance(pose.cell, entity.cell) > 4;
    if (teleport || entity.move_interval == 0 || entity.kind == EntityKind::Projectile) {
        pose.previous_camera = pose.camera_position = entity.cell;
        pose.camera_steps_remaining = 0;
        pose.camera_guide_ready = true;
        return;
    }
    pose.previous_camera = pose.camera_position;
    // CAMERA ONLY: Finish the guide within the movement beat; actors remain on their actual tiles.
    if (pose.cell != entity.cell) pose.camera_steps_remaining = std::clamp(entity.move_interval, 1, 12);
    if (pose.camera_steps_remaining > 0) {
        const float remaining = static_cast<float>(pose.camera_steps_remaining--);
        pose.camera_position.x += (static_cast<float>(entity.cell.x) - pose.camera_position.x) / remaining;
        pose.camera_position.y += (static_cast<float>(entity.cell.y) - pose.camera_position.y) / remaining;
    } else pose.camera_position = entity.cell;
}

ViewCamera camera_guide_position(const EntityPose& pose, float alpha) {
    alpha = std::clamp(alpha, 0.0F, 1.0F);
    return {std::lerp(pose.previous_camera.x, pose.camera_position.x, alpha),
            std::lerp(pose.previous_camera.y, pose.camera_position.y, alpha)};
}
