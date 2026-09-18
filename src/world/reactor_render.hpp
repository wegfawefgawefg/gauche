#pragma once
#include "renderer/device.hpp"
#include "reactor.hpp"
#include "../view.hpp"
#include <SDL3/SDL.h>
void draw_reactor_hazards(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,ViewCamera camera,float zoom);
void draw_reactor_status(tr::Renderer* renderer,const Game& game);
