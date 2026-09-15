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
    int half_width = 0;
    int heal = 0;
    bool piercing = false;
    bool cone = false;
    bool cross_blast = false;
    bool chain = false;
    bool conduction = false;
    int momentum_tip = 0; // Conditional forward cell beyond the ordinary sweep.
};

int pattern_half_width(ItemPattern pattern, int reach);
ItemPattern item_pattern(ItemKind kind);
ItemPattern item_pattern(const Item& item);
ItemPattern active_item_pattern(const Item& item, const Entity& user);
Cell aimed_item_target(const Entity& user, Cell aim, ItemPattern pattern);
