#pragma once
#include "../game.hpp"
#include "../view.hpp"

struct Cosmetics;
void draw_contact_shadows(SDL_Renderer* renderer, const Game& game,
                          const Cosmetics* cosmetics, ViewCamera camera, float zoom);
