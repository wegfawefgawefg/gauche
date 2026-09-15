#pragma once
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_mold_details(SDL_Renderer* renderer,const GameGraphics& graphics,const Entity& actor,SDL_FRect rect,LightColor light);
void apply_mold_pose(const Entity& actor,SDL_FRect& rect,double& angle);
