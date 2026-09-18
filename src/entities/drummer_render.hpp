#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"

void draw_drummer_warning(tr::Renderer* renderer, const Game& game, const Entity& drummer,
    ViewCamera camera, float zoom, const LightingCache& lighting);
