#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../view.hpp"

struct Cosmetics;
void draw_contact_shadows(tr::Renderer* renderer, const Game& game,
                          const Cosmetics* cosmetics, ViewCamera camera, float zoom);
