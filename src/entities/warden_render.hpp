#pragma once
#include "renderer/device.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"
#include "../game.hpp"

void draw_warden_charge(tr::Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Entity& warden, ViewCamera camera, float zoom, const LightingCache& lighting);
