#pragma once
#include "renderer/device.hpp"
#include "../game.hpp"
#include "../lighting/field.hpp"
void apply_freight_pose(const Entity& actor,SDL_FRect& rect,double& angle);
void draw_freight_details(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& actor,SDL_FRect rect,LightColor light);
