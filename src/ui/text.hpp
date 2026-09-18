#pragma once
#include "renderer/device.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <string_view>

void small_ui_text(tr::Renderer* renderer, float x, float y,
                   std::string_view value, std::uint8_t red = 235,
                   std::uint8_t green = 230, std::uint8_t blue = 214);
