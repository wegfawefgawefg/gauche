#pragma once

#include "game.hpp"

const char* item_attribute_name(ItemAttribute attribute);
bool item_accepts_attribute(ItemKind kind, ItemAttribute attribute);
bool item_is_gun(ItemKind kind);
bool item_is_melee(ItemKind kind);
