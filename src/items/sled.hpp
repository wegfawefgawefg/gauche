#pragma once
#include "catalog.hpp"

const RegionalItem* sled_item(ItemKind kind);
void init_sled(Entity& sled);
bool place_sled(Game& game,int user_slot,Cell direction,const Item& item);
void board_sled(Game& game,int player_slot);
const Entity* ridden_sled(const Game& game,const Entity& player);
void sled_player_input(Game& game,int player_slot,Input& input);
void step_sled(Game& game,int slot);
void sled_contact(Game& game,int slot);
void clear_sled_links(Game& game,Entity& sled);
bool sled_cargo(const Game& game,const Entity& cargo);
void step_sled_cargo(Game& game,int slot);
Item recoverable_sled(const Game& game,Cell cell);
int release_sled(Game& game,Cell cell);
bool hit_sled(Game& game,Cell cell,int damage,Cell source);
bool valid_sled(const Entity& sled);
