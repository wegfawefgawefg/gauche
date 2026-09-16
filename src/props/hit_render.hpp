#pragma once
#include "../graphics.hpp"
#include "../lighting/field.hpp"
#include "../game.hpp"
struct Cosmetics;
SDL_FRect jolted_prop_rect(SDL_FRect rect,Cell cell,const Cosmetics* cosmetics);
void draw_wood_splits(SDL_Renderer* renderer,const GameGraphics& graphics,
    SDL_FRect rect,Cell cell,int hp,int max_hp,int turns,const LightingCache& lighting);
