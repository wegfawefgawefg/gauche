#pragma once
#include "projectile.hpp"
#include "../lighting/field.hpp"
#include "../view.hpp"

struct ProjectilePose { SDL_FRect ground; SDL_FRect body; float height = 0; };
ProjectilePose projectile_pose(const Entity& shot,const Game& game,ViewCamera camera,float zoom);

void draw_projectile(SDL_Renderer* renderer, const GameGraphics& graphics,
                     const Entity& shot, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting);
