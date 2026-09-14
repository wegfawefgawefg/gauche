#pragma once
#include "system.hpp"

void step_camera_guide(EntityPose& pose, const Entity& entity, bool same);
ViewCamera camera_guide_position(const EntityPose& pose, float alpha);
