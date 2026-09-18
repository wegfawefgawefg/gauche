#pragma once
#include "renderer/device.hpp"
#include "render.hpp"
void draw_conveyor(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,
                   SDL_FRect rect,LightColor light,std::uint64_t tick);
