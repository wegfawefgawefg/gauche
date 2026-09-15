#pragma once
#include "../lighting/render.hpp"
bool draw_freight_track(SDL_Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
                        Cell cell,SDL_FRect rect,const LightingCache& lighting);
void draw_rail_points(SDL_Renderer* renderer,const GameGraphics& graphics,const Prop& prop,
                      SDL_FRect rect,LightColor light);
