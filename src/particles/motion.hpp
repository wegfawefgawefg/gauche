#pragma once
#include "system.hpp"

void step_actor_motion(EntityPose& pose, const Entity& entity, bool same);
ViewCamera presented_position(const EntityPose& pose, float alpha);
