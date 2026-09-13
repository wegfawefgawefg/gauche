#pragma once

#include "../game.hpp"
#include "../input.hpp"

struct ItemRange { int minimum = 0; int maximum = 0; };
ItemRange item_range(ItemKind kind);
void draw_item_range_base(SDL_Renderer* renderer, const Entity& player, Cell camera, float zoom);
void draw_item_range_top(SDL_Renderer* renderer, const Entity& player, Cell camera, float zoom,
                         const PointerState& pointer, const GameGraphics& graphics);
void draw_hud(SDL_Renderer* renderer, const GameGraphics& graphics,
              const Game& game, const Entity& player);
void draw_interlude(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, int local_owner);
void draw_pointer(SDL_Renderer* renderer, const GameGraphics& graphics,
                  const PointerState& pointer);
