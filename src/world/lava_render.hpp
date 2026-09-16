#pragma once
#include "../lighting/render.hpp"
#include "../game.hpp"

inline Sprite lava_sprite(std::uint64_t tick) {
    return static_cast<Sprite>(static_cast<int>(Sprite::LavaFlow0)+static_cast<int>((tick/12)%8));
}
bool draw_lava(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,Cell cell,
               SDL_FRect rect,const LightingCache& lighting);
