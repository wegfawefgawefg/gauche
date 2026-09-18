#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_steam_drive(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,SDL_FRect rect,LightColor light,std::uint64_t tick);
