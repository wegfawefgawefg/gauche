#pragma once
#include "render.hpp"
void draw_conveyor(SDL_Renderer* renderer,const GameGraphics& graphics,const Prop& prop,
                   SDL_FRect rect,LightColor light,std::uint64_t tick);
