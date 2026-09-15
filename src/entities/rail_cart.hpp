#pragma once
#include "../game.hpp"
void init_rail_cart(Entity& cart);
void step_rail_cart(Game& game,int slot);
bool push_rail_cart(Game& game,int slot,Cell direction,Handle pusher={});
bool haul_rail_cart(Game& game,int slot,Cell direction);
bool brake_rail_cart(Game& game,Cell cell);
void hurt_rail_cart(Game& game,Entity& cart,int damage);
Item recoverable_cart_item(const Game& game,Cell cell);
int release_cart_item(Game& game,Cell source,Cell destination);
bool finish_cart_death(Game& game,int slot);
bool valid_rail_cart(const Entity& cart);
