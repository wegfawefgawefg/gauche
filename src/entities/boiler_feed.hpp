#pragma once
#include "../game.hpp"
constexpr int boiler_water_limit=600;
BoilerFeed* boiler_feed(Game& game,const Entity& tank);
const BoilerFeed* boiler_feed(const Game& game,const Entity& tank);
bool boiler_feed_connected(const Game& game,const BoilerFeed& feed);
bool step_boiler_feed(Game& game,Entity& tank); // True owns installed pressure/fuel changes.
bool consume_boiler_water(Game& game,const Entity& tank,bool rupture);
bool repair_feed_fixture(Game& game,Cell cell);
bool valid_boiler_feeds(const Game& game);
