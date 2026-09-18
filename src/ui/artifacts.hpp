#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"

void draw_owned_artifacts(tr::Renderer* renderer, const GameGraphics& graphics,
                          const Entity& player, float x, float y, bool details);
