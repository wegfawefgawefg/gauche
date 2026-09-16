#pragma once
#include "roof.hpp"
#include "../lighting/render.hpp"
#include <optional>

bool reveal_roof(const RoofSpan& roof,Cell viewer);
std::optional<Sprite> log_far_support(const Stage& stage,Cell cell);
void draw_roofs(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);
