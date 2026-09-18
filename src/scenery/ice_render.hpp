#pragma once
#include "renderer/device.hpp"
#include "../particles/system.hpp"
void draw_ice_scenery(tr::Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Cosmetics& cosmetics, ViewCamera camera, float zoom,
    const LightingCache& lighting);
void observe_ice_scenery(Cosmetics& cosmetics, const Game& game, Cell focus);
