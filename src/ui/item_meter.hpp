#pragma once
#include "renderer/device.hpp"

#include "../game.hpp"

#include <SDL3/SDL.h>

#include <string>

std::string item_cooldown_text(const Item& item);
std::string item_state_text(const Item& item, bool compact = false);
int item_meter_capacity(const Item& item);
int item_meter_current(const Item& item);
void draw_item_meter(tr::Renderer* renderer, float x, float y,
                     float width, float height, int current, int maximum,
                     SDL_Color color);

void draw_muffled_count(tr::Renderer* renderer, const Item& item, float x, float y);
