#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"

struct Cosmetics;

void draw_props(tr::Renderer* renderer, const GameGraphics& graphics, const Game& game,
                 ViewCamera camera, float zoom, const LightingCache& lighting, std::uint64_t tick=0, const Cosmetics* cosmetics=nullptr);
