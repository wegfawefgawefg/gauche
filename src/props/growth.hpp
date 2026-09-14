#pragma once

#include "interaction.hpp"

bool plant_prop(Game& game, Cell cell, PropKind kind);
void step_prop_growth(Game& game);
LightEmitter prop_light(const Prop& prop);
