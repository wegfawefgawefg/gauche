#pragma once
#include "../game.hpp"

void draw_owned_artifacts(SDL_Renderer* renderer, const GameGraphics& graphics,
                          const Entity& player, float x, float y, bool details);
