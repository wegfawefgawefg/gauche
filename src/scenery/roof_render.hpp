#pragma once
#include "roof.hpp"
#include "../lighting/render.hpp"
#include <optional>

bool reveal_roof(const RoofSpan& roof,Cell viewer);
std::optional<Sprite> log_far_support(const Stage& stage,Cell cell);
struct RoofGround { Sprite sprite; int quarter_turns; };
std::optional<RoofGround> roof_ground(const Stage& stage,Cell cell);
int log_support_turns(const Stage& stage,Cell cell);
void draw_roofs(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);

void draw_roof_row(SDL_Renderer* renderer,const GameGraphics& graphics,const RoofSpan& roof,int row,
                   const Entity* viewer,ViewCamera camera,float zoom,const LightingCache& lighting);
