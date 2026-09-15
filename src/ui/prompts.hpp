#pragma once
#include "../input/prompts.hpp"
#include <string_view>

float draw_prompt(SDL_Renderer* renderer, float x, float y, const InputPrompt& prompt);
void draw_action_hint(SDL_Renderer* renderer, float x, float y, Action action, std::string_view label);
void draw_modal_hint(SDL_Renderer* renderer, float x, float y, bool cancel, std::string_view label);
