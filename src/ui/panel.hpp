#pragma once
#include <SDL3/SDL.h>

void angled_fill(SDL_Renderer* renderer, SDL_FRect rect, SDL_FColor top, SDL_FColor bottom);
void angled_panel(SDL_Renderer* renderer, SDL_FRect rect, SDL_Color edge);
