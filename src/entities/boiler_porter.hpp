#pragma once
#include "../game.hpp"

enum PorterPhase { PorterReady, PorterPush, PorterBite, PorterRest };
void init_boiler_porter(Entity& porter);
void step_boiler_porter(Game& game, int slot);
void interrupt_boiler_porter(Entity& porter);
Handle spawn_boiler_porter(Game& game, Cell cell);
