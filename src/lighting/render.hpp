#pragma once

#include "field.hpp"

#include <SDL3/SDL.h>

void draw_lit_tile(SDL_Renderer* renderer, SDL_Texture* texture,
                   SDL_FRect rect, Cell cell, const LightingCache& lighting,
                   LightColor tint = {1.0F, 1.0F, 1.0F},
                   SDL_FRect uv = {0,0,1,1}, int quarter_turns = 0,
                   float opacity = 1.0F, bool flip_horizontal = false);
LightColor lit_sprite_color(const LightingCache& lighting, Cell cell,
                            LightColor self_glow = {});
