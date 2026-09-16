#pragma once
#include "../game.hpp"
constexpr std::size_t max_driven_belts=128;
const BoilerFeed* belt_supply(const Game& game,Cell cell);
bool live_steam_drive(const Game& game,const BoilerFeed& feed);
bool steam_drive_powered(const Game& game,const BoilerFeed& feed);
bool driven_belt_powered(const Game& game,const BoilerFeed& feed,Cell cell);
bool cutter_powered(const Game& game,const Entity& cutter);
bool valid_boiler_drives(const Game& game);
