#pragma once

#include "../item_pattern.hpp"

enum class ItemAction { Melee, Gun, Throw, Material, Food };
struct RegionalItem {
    const char* name;
    const char* description;
    Sprite sprite;
    ItemPattern pattern;
    ItemAction action;
    int price = 0;
    int max_count = 1;
    bool consume = false;
    int uses = 0;
    int magazine = 0;
    int spare = 0;
    int reload = 60;
    int dig_power = 0;
    SoundId sound = SoundId::Punch1;
    int durability = 0;
    LightEmitter light{};
};

const RegionalItem* regional_item(ItemKind kind);
bool strike_melee(Game& game, int user_slot, Cell direction, const Item& item);
const RegionalItem* forest_food_item(ItemKind kind);
