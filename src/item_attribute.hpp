#pragma once

#include "game.hpp"

#include <string>

const char* item_attribute_name(ItemAttribute attribute);
const char* item_attribute_effect(ItemAttribute attribute);
std::string item_display_name(const Item& item);
bool item_accepts_attribute(ItemKind kind, ItemAttribute attribute);
bool item_is_gun(ItemKind kind);
bool item_is_melee(ItemKind kind);
