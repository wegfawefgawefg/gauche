#pragma once

#include "../game.hpp"

void draw_item_flame(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Item& item, SDL_FRect rect, Cell facing, std::uint64_t tick);
