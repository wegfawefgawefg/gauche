#pragma once
#include "catalog.hpp"

const RegionalItem* muffling_item(ItemKind kind);
bool muffleable_item(const Item& item);
int muffling_target_slot(const Inventory& inventory);
bool apply_muffling(Game& game, int slot);
void emit_weapon_sound(Game& game, const Item& item, SoundId sound, Cell cell);
void finish_muffled_use(Game& game, Item& item, Cell cell);
