#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"
#include "../input.hpp"
#include "../view.hpp"

struct ItemRange { int minimum = 0; int maximum = 0; };
ItemRange item_range(ItemKind kind);
void draw_item_range_top(tr::Renderer* renderer, const GameGraphics& graphics,
                         const Game& game, const Entity& player, ViewCamera camera,
                         float zoom, const PointerState& pointer);
void draw_hud(tr::Renderer* renderer, const GameGraphics& graphics,
              const Game& game, const Entity& player,
              const PointerState& pointer, bool compact_details);
void draw_pointer(tr::Renderer* renderer, const GameGraphics& graphics,
                  const PointerState& pointer);
void draw_window_pointer(tr::Renderer* renderer, SDL_Window* window, const GameGraphics& graphics);
