#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_pump_nozzle(SDL_Renderer* renderer,const Entity& actor,SDL_FRect rect,LightColor light);
void apply_pump_pose(const Entity& actor,SDL_FRect& rect);
