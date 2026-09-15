#include "interaction.hpp"
#include "../items/ground_interaction.hpp"
#include "../input.hpp"
#include "scale.hpp"

#include <algorithm>

namespace {

bool offering(const Game& game, int owner) {
    return game.run.phase == RunPhase::Reward ||
        (game.run.phase == RunPhase::Playing && owner >= 0 && owner < 4 &&
         game.run.pending_count[static_cast<std::size_t>(owner)] > 0);
}

bool reward_fits(const Game& game, int owner, int index) {
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr) return false;
    const Reward reward = reward_offer(game, owner, index);
    if (reward.kind != RewardKind::Item) return true;
    Inventory trial = player->inventory;
    return insert_item(trial, reward_item(reward));
}

bool shop_fits(const Game& game, int owner, int index) {
    if (index < 0 || index >= 3) return false;
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr) return false;
    Inventory trial = player->inventory;
    return insert_item(trial, make_item(game.run.shop_stock[static_cast<std::size_t>(index)]));
}

void reset_if_run_changed(InteractionUi& ui, const Game& game) {
    if (game.tick < ui.last_tick || game.run.phase != ui.last_phase) {
        ui.inventory_open = false;
        ui.compare_ground = false;
        ui.offer_focus = 0;
        ui.notice.clear();
    }
    ui.last_tick = game.tick;
    ui.last_phase = game.run.phase;
}

int logical_x(const SDL_Event& event, const GubsyFrame& frame) {
    int window_width = 0;
    int window_height = 0;
    SDL_GetWindowSize(frame.window, &window_width, &window_height);
    if (window_width <= 0 || window_height <= 0) return -1;
    const float scale = std::min(static_cast<float>(window_width) / 640.0F,
                                 static_cast<float>(window_height) / 360.0F);
    const float left = (static_cast<float>(window_width) - 640.0F * scale) * 0.5F;
    const float x = event.type == SDL_EVENT_MOUSE_MOTION ? event.motion.x : event.button.x;
    return static_cast<int>((x - left) / scale);
}

int logical_y(const SDL_Event& event, const GubsyFrame& frame) {
    int window_width = 0;
    int window_height = 0;
    SDL_GetWindowSize(frame.window, &window_width, &window_height);
    if (window_width <= 0 || window_height <= 0) return -1;
    const float scale = std::min(static_cast<float>(window_width) / 640.0F,
                                 static_cast<float>(window_height) / 360.0F);
    const float top = (static_cast<float>(window_height) - 360.0F * scale) * 0.5F;
    const float y = event.type == SDL_EVENT_MOUSE_MOTION ? event.motion.y : event.button.y;
    return static_cast<int>((y - top) / scale);
}

} // namespace

bool has_reward_offer(const Game& game, int owner) { return offering(game, owner); }

Reward reward_offer(const Game& game, int owner, int index) {
    if (owner < 0 || owner >= 4 || index < 0 || index >= 3) return {};
    const auto row = static_cast<std::size_t>(owner);
    const auto column = static_cast<std::size_t>(index);
    if (game.run.phase == RunPhase::Playing && game.run.pending_count[row] > 0)
        return game.run.pending_offers[row][0][column];
    return game.run.offers[row][column];
}

bool interaction_event(InteractionUi& ui, const SDL_Event& event,
                       const GubsyFrame& frame, const Game& game, int owner) {
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && ui.inventory_open) {
        if (event.key.key == SDLK_ESCAPE) { ui.request_back = true; return true; }
        if (event.key.key == SDLK_DELETE || event.key.key == SDLK_BACKSPACE) {
            ui.request_drop = true; return true;
        }
        if (event.key.key == SDLK_UP) {
            ui.slot_focus = (ui.slot_focus + quick_slots - 1) % quick_slots; return true;
        }
        if (event.key.key == SDLK_DOWN) {
            ui.slot_focus = (ui.slot_focus + 1) % quick_slots; return true;
        }
    }
    if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN && ui.inventory_open) {
        if (event.gbutton.button == SDL_GAMEPAD_BUTTON_EAST) {
            ui.request_back = true; return true;
        }
    }
    if (!ui.inventory_open && (offering(game, owner) || game.run.phase == RunPhase::Shop)) {
        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            if (event.key.key == SDLK_LEFT) {
                const int count = game.run.phase == RunPhase::Shop ? 4 : 3;
                ui.offer_focus = (ui.offer_focus + count - 1) % count; return true;
            }
            if (event.key.key == SDLK_RIGHT) {
                const int count = game.run.phase == RunPhase::Shop ? 4 : 3;
                ui.offer_focus = (ui.offer_focus + 1) % count; return true;
            }
        }
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION && !ui.inventory_open && frame.window &&
        (offering(game, owner) || game.run.phase == RunPhase::Shop)) {
        const float x = (static_cast<float>(logical_x(event, frame)) - modal_left) / ui_scale;
        const float y = (static_cast<float>(logical_y(event, frame)) - modal_top) / ui_scale;
        if (x >= 20 && x < 620 && y >= 65 && y <= 306)
            ui.offer_focus = std::clamp(static_cast<int>((x - 20) / 202), 0, 2);
        return true;
    }
    if (event.type != SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event.button.button != SDL_BUTTON_LEFT || frame.window == nullptr) return false;
    const int x = static_cast<int>((static_cast<float>(logical_x(event, frame)) - modal_left) / ui_scale);
    const int y = static_cast<int>((static_cast<float>(logical_y(event, frame)) - modal_top) / ui_scale);
    if (ui.inventory_open) {
        if (x >= 20 && x <= 180 && y >= 60 && y < 300)
            ui.mouse_slot = std::clamp((y - 60) / 39, 0, quick_slots - 1);
        else if (y >= 315 && y <= 355 && x >= 25 && x < 215) ui.request_drop = true;
        else if (y >= 315 && y <= 355 && x >= 420 && x < 640) ui.request_back = true;
        return true;
    }
    if ((offering(game, owner) || game.run.phase == RunPhase::Shop) &&
        y >= 65 && y <= 306 && x >= 20 && x < 620) {
        ui.mouse_choice = std::clamp((x - 20) / 202, 0, 2);
        return true;
    }
    if (game.run.phase == RunPhase::Shop && y >= 315 && y <= 355 && x >= 430 && x < 640) {
        ui.mouse_choice = 3;
        return true;
    }
    return false;
}

void apply_interaction_input(InteractionUi& ui, const Game& game, int owner,
                             GubsyRuntime& runtime, Input& input) {
    reset_if_run_changed(ui, game);
    const bool offered = offering(game, owner);
    const bool shop = game.run.phase == RunPhase::Shop;
    const bool open_down = inventory_button_down(runtime);
    const bool compare_down = compare_button_down(runtime);
    const bool compact_down = compact_details_button_down(runtime);
    if (compact_down && !ui.compact_latch) ui.compact_details = !ui.compact_details;
    ui.compact_latch = compact_down;
    if (open_down && !ui.inventory_latch) {
        ui.inventory_open = !ui.inventory_open;
        ui.compare_ground = false;
        ui.slot_focus = 0;
        ui.notice.clear();
    }
    ui.inventory_latch = open_down;
    if (compare_down && !ui.compare_latch) {
        ui.inventory_open = !ui.inventory_open;
        ui.compare_ground = false;
        if (ui.inventory_open && !offered && !shop) {
            const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
            if (player != nullptr)
                ui.compare_ground = reachable_pickup_item(game,*player).kind != ItemKind::None;
        }
        ui.slot_focus = 0;
        ui.notice.clear();
    }
    ui.compare_latch = compare_down;
    if (ui.request_back) ui.inventory_open = false;
    ui.request_back = false;
    if (ui.mouse_slot >= 0) ui.slot_focus = ui.mouse_slot;
    ui.mouse_slot = -1;
    const bool confirm = input.confirm && !ui.confirm_latch;
    const bool pickup = input.pickup && !ui.pickup_latch;
    ui.confirm_latch = input.confirm;
    ui.pickup_latch = input.pickup;
    input.pickup = pickup;
    const int nav = ui.inventory_open ? input.move.y : input.move.x;
    if (nav != 0 && nav != ui.move_latch) {
        if (ui.inventory_open)
            ui.slot_focus = (ui.slot_focus + quick_slots + nav) % quick_slots;
        else if (offered || shop) {
            const int count = shop ? 4 : 3;
            ui.offer_focus = (ui.offer_focus + count + nav) % count;
        }
    }
    ui.move_latch = nav;
    if (ui.offer_focus != ui.previous_offer_focus) {
        ui.previous_offer_focus = ui.offer_focus;
        ui.offer_changed_at = SDL_GetTicks();
    }
    ui.slide += ((ui.inventory_open ? 1.0F : 0.0F) - ui.slide) * 0.25F;
    if (!ui.inventory_open && !offered && !shop) {
        ui.mouse_choice = -1;
        ui.request_drop = false;
        return;
    }
    const int number = input.select;
    const bool drop = ui.request_drop || (ui.inventory_open && pickup);
    ui.request_drop = false;
    input = {};
    input.cancel_use = true;
    if (ui.inventory_open) {
        if (number >= 0 && number < quick_slots) ui.slot_focus = number;
        if (drop) {
            const Entity* player = get_entity(game,
                game.players[static_cast<std::size_t>(owner)]);
            const ItemKind kind = player == nullptr ? ItemKind::None :
                player->inventory.slots[static_cast<std::size_t>(ui.slot_focus)].kind;
            if (kind != ItemKind::None && kind != ItemKind::Fist) {
                input.select = ui.slot_focus;
                input.drop = true;
                ui.notice = "Dropped selected item";
            } else ui.notice = "This slot cannot be dropped";
        } else if (confirm && !offered && !shop) {
            input.select = ui.slot_focus;
            ui.notice = "Held item changed";
        }
        ui.mouse_choice = -1;
        return;
    }
    const int choice = ui.mouse_choice >= 0 ? ui.mouse_choice :
        (number >= 0 && number < 3 ? number : (confirm ? ui.offer_focus : -1));
    ui.mouse_choice = -1;
    if (choice >= 0) {
        ui.offer_focus = choice;
        if (shop && choice == 3) { input.confirm = true; return; }
        if (offered && game.run.phase == RunPhase::Reward &&
            game.run.chosen[static_cast<std::size_t>(owner)]) return;
        if (offered && !reward_fits(game, owner, choice)) {
            ui.inventory_open = true;
            ui.notice = "Pack full. Select and drop an item, then choose again.";
            return;
        }
        if (shop && game.run.shop_stock[static_cast<std::size_t>(choice)] != ItemKind::None &&
            !shop_fits(game, owner, choice)) {
            ui.inventory_open = true;
            ui.notice = "Pack full. Drop an item before buying.";
            return;
        }
        input.select = choice;
    }
}
