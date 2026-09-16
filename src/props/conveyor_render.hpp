#pragma once
#include "render.hpp"
void draw_conveyor(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,
                   SDL_FRect rect,LightColor light,std::uint64_t tick);
