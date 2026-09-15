#pragma once
#include "../game.hpp"

Reward current_offer(const Game& game, int owner, int choice);
std::uint64_t offer_token(const Game& game, int owner, int choice);
void apply_offer_choice(Game& game, int owner, const Input& input);
bool accept_offer_item(Game& game, Entity& player, Item incoming,
                       int replace_slot, ItemKind expected);
