#pragma once

#include "../game.hpp"

constexpr int stick_flame_duration = 30 * 60;
LightEmitter item_light(const Item& item);
bool light_stick(Game& game, Item& item, Cell source);
void step_item_state(Game& game, Item& item, Cell cell, bool wet);
void ignite_struck_actor(Game& game, int slot);
