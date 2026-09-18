#pragma once
#include "renderer/device.hpp"
#include "../particles/system.hpp"

void draw_overhead(tr::Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, const Cosmetics* cosmetics, ViewCamera camera,
                    float zoom, const LightingCache& lighting);
