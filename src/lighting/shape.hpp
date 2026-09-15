#pragma once
#include "../game.hpp"

#include <algorithm>
#include <cstdlib>

// AIM: Shared integer cone edge for source-seeking actors and local light rendering.
inline int light_direction_factor(LightShape shape,Cell facing,Cell offset) {
    if (shape==LightShape::Omni || offset==Cell{}) return 1000;
    const int forward=offset.x*facing.x+offset.y*facing.y;
    if (forward<=0) return 0;
    const int side=std::abs(offset.x*facing.y-offset.y*facing.x);
    const int edge=forward*(shape==LightShape::Cone ? 4 : 2)-side*4;
    return std::clamp(edge*1000/std::max(4,forward),0,1000);
}
