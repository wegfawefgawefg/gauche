#pragma once

#include "../game.hpp"

#include <gubsy/runtime.hpp>

#include <string>

struct InteractionUi {
    bool inventory_open = false;
    bool inventory_latch = false;
    bool compare_latch = false;
    bool compact_latch = false;
    bool compact_details = true;
    bool compare_ground = false;
    bool confirm_latch = false;
    bool pickup_latch = false;
    int move_latch = 0;
    int slot_focus = 0;
    int offer_focus = 0;
    int previous_offer_focus = -1;
    std::uint64_t offer_changed_at = 0;
    int mouse_choice = -1;
    int mouse_slot = -1;
    bool request_drop = false;
    bool request_back = false;
    float slide = 0.0F;
    RunPhase last_phase = RunPhase::Arena;
    std::uint64_t last_tick = 0;
    std::string notice;
};

bool has_reward_offer(const Game& game, int owner);
Reward reward_offer(const Game& game, int owner, int index);
bool interaction_event(InteractionUi& ui, const SDL_Event& event,
                       const GubsyFrame& frame, const Game& game, int owner);
void apply_interaction_input(InteractionUi& ui, const Game& game, int owner,
                             GubsyRuntime& runtime, Input& input);
void draw_interaction(SDL_Renderer* renderer, const GameGraphics& graphics,
                      const Game& game, int owner, const InteractionUi& ui);
