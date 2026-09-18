#pragma once
#include "renderer/device.hpp"
#include "../lighting/render.hpp"
bool draw_freight_track(tr::Renderer* renderer,const GameGraphics& graphics,const Stage& stage,
                        Cell cell,SDL_FRect rect,const LightingCache& lighting);
void draw_rail_points(tr::Renderer* renderer,const GameGraphics& graphics,const Prop& prop,
                      SDL_FRect rect,LightColor light);
