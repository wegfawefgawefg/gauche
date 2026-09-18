#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/render.hpp"

bool draw_chasm(tr::Renderer* renderer,const Game& game,Cell cell,SDL_FRect rect,const LightingCache& lighting);
