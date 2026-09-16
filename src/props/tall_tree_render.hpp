#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
struct Cosmetics;
void draw_tall_tree(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_tree_ground(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting,const Cosmetics* cosmetics=nullptr);
