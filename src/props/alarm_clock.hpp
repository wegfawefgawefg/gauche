#pragma once
#include "../game.hpp"

constexpr int alarm_delay_ticks = 180;
constexpr int alarm_ring_ticks = 300;
bool place_alarm_clock(Game& game, Cell cell, const Item& item);
void step_alarm_clock(Game& game, Cell cell);
Sprite alarm_clock_sprite(const Prop& prop);
