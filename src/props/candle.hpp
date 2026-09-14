#pragma once
#include "../game.hpp"

constexpr int candle_fuel_ticks = 4800;
constexpr int candle_lit_bit = 1;
constexpr int candle_durable_bit = 2;
bool candle_lit(const Prop& prop);
bool light_candle(Game& game, Cell cell);
bool douse_candle(Game& game, Cell cell);
bool place_candle(Game& game, Cell cell, const Item& item);
bool refill_candle(Game& game, Cell cell);
void step_candle(Game& game, Cell cell);
Item candle_item(const Prop& prop);
int release_candle(Game& game, Cell cell);
