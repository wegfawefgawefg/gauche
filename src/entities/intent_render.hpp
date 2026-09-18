#pragma once
#include "renderer/device.hpp"

#include "../lighting/render.hpp"
#include "../view.hpp"

void draw_enemy_intents(tr::Renderer* renderer, const Game& game,
                        ViewCamera camera, float zoom, const LightingCache& lighting);
