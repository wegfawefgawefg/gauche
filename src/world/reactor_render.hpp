#pragma once
#include "reactor.hpp"
#include "../view.hpp"
#include <SDL3/SDL.h>
void draw_reactor_hazards(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,ViewCamera camera,float zoom);
void draw_reactor_status(SDL_Renderer* renderer,const Game& game);
