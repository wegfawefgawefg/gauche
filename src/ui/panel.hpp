#pragma once
#include "renderer/device.hpp"
#include <SDL3/SDL.h>

void angled_fill(tr::Renderer* renderer, SDL_FRect rect, SDL_FColor top, SDL_FColor bottom);
void angled_panel(tr::Renderer* renderer, SDL_FRect rect, SDL_Color edge);
