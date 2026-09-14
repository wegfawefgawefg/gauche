#include "motion.hpp"

#include <algorithm>
#include <cmath>

namespace {

ViewCamera travel_center(EntityPose& pose, const Entity& entity) {
    const int samples = static_cast<int>(pose.camera_samples.size());
    pose.camera_sample_index = (pose.camera_sample_index + 1) % samples;
    pose.camera_samples[static_cast<std::size_t>(pose.camera_sample_index)] = entity.cell;
    // STRIDE PAIRS: Average two movement beats so right/up alternation describes
    // a diagonal camera path. A spring alone still traces a softened staircase.
    if (pose.cell != entity.cell)
        pose.camera_stride = std::clamp(movement_beat(entity, std::max(entity.move_interval, entity.move_wait)), 1, samples / 2);
    const int window = std::clamp(pose.camera_stride * 2, 2, samples);
    ViewCamera center{};
    for (int age = 0; age < window; ++age) {
        const int index = (pose.camera_sample_index + samples - age) % samples;
        const ViewCamera sample = pose.camera_samples[static_cast<std::size_t>(index)];
        center.x += sample.x; center.y += sample.y;
    }
    center.x /= static_cast<float>(window); center.y /= static_cast<float>(window);
    return center;
}

void follow_axis(float& position, float& velocity, float target) {
    // CAMERA INERTIA: A critically damped follow keeps velocity through the
    // alternating cardinal steps of diagonal input, without overshooting a stop.
    constexpr float dt = 1.0F / 60.0F;
    constexpr float frequency = 24.0F;
    const float decay = std::exp(-frequency * dt);
    const float offset = position - target;
    const float change = (velocity + frequency * offset) * dt;
    position = target + (offset + change) * decay;
    velocity = (velocity - frequency * change) * decay;
}

} // namespace

void step_camera_guide(EntityPose& pose, const Entity& entity, bool same) {
    const bool teleport = !same || !pose.camera_guide_ready || distance(pose.cell, entity.cell) > 4;
    if (teleport || entity.kind != EntityKind::Player) {
        pose.previous_camera = pose.camera_position = entity.cell;
        pose.camera_velocity = {};
        pose.camera_samples.fill(ViewCamera{entity.cell});
        pose.camera_sample_index = 0;
        pose.camera_stride = std::max(1, movement_beat(entity, entity.move_interval));
        pose.camera_guide_ready = true;
        return;
    }
    pose.previous_camera = pose.camera_position;
    const ViewCamera target = travel_center(pose, entity);
    follow_axis(pose.camera_position.x, pose.camera_velocity.x, target.x);
    follow_axis(pose.camera_position.y, pose.camera_velocity.y, target.y);
}

ViewCamera camera_guide_position(const EntityPose& pose, float alpha) {
    alpha = std::clamp(alpha, 0.0F, 1.0F);
    return {std::lerp(pose.previous_camera.x, pose.camera_position.x, alpha),
            std::lerp(pose.previous_camera.y, pose.camera_position.y, alpha)};
}
