#pragma once
#include "../game.hpp"

inline constexpr int hearth_meal_heal = 3;
inline constexpr int hearth_meal_radius = 4;
bool hearth_meal(ItemKind kind);
bool hearth_meal_needed(const Game& game, const Entity& source, ItemKind kind);
void share_hearth_meal(Game& game, const Entity& source, ItemKind kind);
