#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"
void draw_shard_links(tr::Renderer* renderer, const Game& game, ViewCamera camera,
                      float zoom, const LightingCache& lighting);
