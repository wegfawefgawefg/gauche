#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"
void apply_freight_pose(const Entity& actor,SDL_FRect& rect,double& angle);
void draw_freight_details(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& actor,SDL_FRect rect,LightColor light);
