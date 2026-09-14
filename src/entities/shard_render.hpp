#pragma once
#include "../game.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"
void draw_shard_links(SDL_Renderer* renderer, const Game& game, ViewCamera camera,
                      float zoom, const LightingCache& lighting);
