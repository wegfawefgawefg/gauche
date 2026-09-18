#pragma once
#include "renderer/device.hpp"
#include "../render.hpp"
#include "../lighting/field.hpp"
void draw_streetlamp(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
                     Cell cell,ViewCamera camera,float zoom,const LightingCache& lighting);
void draw_streetlamp_shadow(tr::Renderer* renderer,const Stage& stage,Cell cell,ViewCamera camera,float zoom);
void draw_pole_wreck(tr::Renderer* renderer,const Prop& prop,SDL_FRect rect,LightColor light);
