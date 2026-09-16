#pragma once
#include "render.hpp"
#include "scene_order.hpp"
#include "lighting/field.hpp"
void draw_entities(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                   ViewCamera camera,float zoom,const Cosmetics* cosmetics,
                   const LightingCache& lighting,ScenePass pass,const Entity* viewer);
