#pragma once
#include "lava_eruptions.hpp"
#include "../view.hpp"

void draw_lava_eruptions(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                         ViewCamera camera,float zoom,bool airborne);
