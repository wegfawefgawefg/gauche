#pragma once
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_streetlamp(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                     Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_streetlamp_shadow(SDL_Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom);
void draw_pole_wreck(SDL_Renderer* renderer,const Prop& prop,SDL_FRect rect,LightColor light);
