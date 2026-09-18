#pragma once
#include "renderer/device.hpp"

#include "field.hpp"

#include <SDL3/SDL.h>

// A null texture fills the tile with color. Shared edges survive fractional zoom.
void draw_flat_tile(tr::Renderer* renderer, tr::Texture* texture, SDL_FRect rect,
                    SDL_FColor color, SDL_FRect uv = {0,0,1,1},
                    int quarter_turns = 0, bool flip_horizontal = false);
void draw_lit_tile(tr::Renderer* renderer, tr::Texture* texture,
                   SDL_FRect rect, Cell cell, const LightingCache& lighting,
                   LightColor tint = {1.0F, 1.0F, 1.0F},
                   SDL_FRect uv = {0,0,1,1}, int quarter_turns = 0,
                   float opacity = 1.0F, bool flip_horizontal = false);
LightColor lit_sprite_color(const LightingCache& lighting, Cell cell,
                            LightColor self_glow = {});
