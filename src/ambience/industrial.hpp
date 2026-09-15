#pragma once
#include "system.hpp"
void place_industrial_ambience(AmbientAudio& audio,const Game& game,Cell listener);
bool industrial_ambient_active(const AmbientSource& source,const Game& game,Cell listener);
