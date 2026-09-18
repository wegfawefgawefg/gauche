#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"

const char* item_description(ItemKind kind);
void draw_item_details(tr::Renderer* renderer, const GameGraphics& graphics,
                       const Entity& player, const Item& item, float x, float y,
                       float width, float height, const char* label, bool highlight = true);
void draw_compact_item_details(tr::Renderer* renderer, const GameGraphics& graphics,
                               const Item& item, float x, float y,
                               float width, const char* label);
void draw_item_banner(tr::Renderer* renderer, float x, float y, float width,
                      const char* label, bool highlight);
