#pragma once
#include "renderer/device.hpp"
#include "fissures.hpp"
#include "../view.hpp"
#include "../lighting/field.hpp"
void draw_fissures(tr::Renderer* renderer,const GameGraphics& graphics,const Game& game,
                    ViewCamera camera,float zoom,const LightingCache& lighting,bool plumes);
