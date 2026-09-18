#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/field.hpp"
struct Cosmetics;
void draw_tall_tree(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_tree_ground(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
    Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting,const Cosmetics* cosmetics=nullptr);
