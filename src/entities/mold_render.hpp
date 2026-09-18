#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void draw_mold_details(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& actor,SDL_FRect rect,LightColor light);
void apply_mold_pose(const Entity& actor,SDL_FRect& rect,double& angle);
