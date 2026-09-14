#pragma once

#include "game.hpp"

enum class PatternEffect { None, Damage, Heal, Utility };

struct ItemPattern {
    int minimum = 0;
    int maximum = 0;
    int blast_radius = 0;
    int damage = 0;
    int cooldown = 0;
    PatternEffect effect = PatternEffect::None;
    bool ray = false;
};

ItemPattern item_pattern(ItemKind kind);
Cell aimed_item_target(const Entity& user, Cell aim, ItemPattern pattern);
