#include "presentation.hpp"
#include "status.hpp"
#include "item_details.hpp"
#include "item_meter.hpp"
#include "text.hpp"
#include "scale.hpp"
#include "prompts.hpp"
#include "../items/ground_interaction.hpp"
#include "../item_attribute.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

namespace {

void ui_text(SDL_Renderer* renderer, float x, float y, const char* value) {
    small_ui_text(renderer, x, y, value);
}

void panel(SDL_Renderer* renderer, float x, float y, float width, float height,
           bool selected = false) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_FRect shadow{x + 3.0F, y + 3.0F, width, height};
    SDL_SetRenderDrawColor(renderer, 4, 5, 5, 185);
    SDL_RenderFillRect(renderer, &shadow);
    SDL_FRect face{x, y, width, height};
    SDL_SetRenderDrawColor(renderer, selected ? 38 : 19, selected ? 30 : 22,
                           selected ? 26 : 23, 215);
    SDL_RenderFillRect(renderer, &face);
    SDL_SetRenderDrawColor(renderer, selected ? 175 : 75, selected ? 63 : 76,
                           selected ? 55 : 70, 220);
    SDL_RenderRect(renderer, &face);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

} // namespace

void draw_hud(SDL_Renderer* renderer, const GameGraphics& graphics,
              const Game& game, const Entity& player,
              const PointerState& pointer, bool compact_details) {
    const HudScale scale{renderer};
    constexpr float width = 640.0F / ui_scale, height = 360.0F / ui_scale;
    const bool quiet = (SDL_GetModState() & SDL_KMOD_ALT) != 0;
    // The inventory stays clear of the player, with the selected row protruding.
    for (int index = 0; index < quick_slots; ++index) {
        const bool selected = index == player.inventory.selected;
        const InputPrompt slot_key = action_prompt(static_cast<Action>(action_id(Action::Slot1) + index));
        const float key_width = controller_input_active() ? 14 :
            std::max(14.0F, 6 * static_cast<float>(slot_key.label.size()) + 6);
        const float x = key_width + (selected ? 16.0F : 8.0F);
        const float y = 58.0F + static_cast<float>(index) * 20.0F;
        panel(renderer, x, y, 104.0F, 18.0F, selected);
        if (!controller_input_active()) draw_prompt(renderer, 2, y + 3, slot_key);
        const Item& item = player.inventory.slots[static_cast<std::size_t>(index)];
        if (item.kind != ItemKind::None) {
            SDL_FRect icon{x + 3.0F, y + 3.0F, 12.0F, 12.0F};
            SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item)),
                              nullptr, &icon);
            if (!quiet) {
                const std::string label = item.attribute == ItemAttribute::None ?
                    item_name(item.kind) :
                    std::string{item_attribute_name(item.attribute), 1} +
                    " " + item_name(item.kind);
                small_ui_text(renderer, x + 19.0F, y + 1.0F,
                              label.substr(0, 13), 235, 230, 214);
            }
            const std::string state = item_state_text(item, true);
            small_ui_text(renderer, x + 19.0F, y + 9.0F,
                          state.substr(0, 6), 200, 207, 189);
            small_ui_text(renderer, x + 60.0F, y + 9.0F,
                          item_cooldown_text(item), 217, 183, 128);
            draw_item_meter(renderer, x + 19.0F, y + 16.0F, 38.0F, 2.0F,
                            item_meter_current(item), item_meter_capacity(item),
                            {139, 190, 134, 255});
            draw_item_meter(renderer, x + 60.0F, y + 16.0F, 42.0F, 2.0F,
                            item.cooldown, item_pattern(item).cooldown,
                            {218, 156, 79, 255});
        }
        if (selected) {
            SDL_FRect arrow{x - 14.0F, y + 3.0F, 11.0F, 11.0F};
            SDL_RenderTexture(renderer, texture_for(graphics, Sprite::SelectedArrow),
                              nullptr, &arrow);
        }
    }

    // Rust's offset red bar is compacted to the half-size render target.
    panel(renderer, 14.0F, height - 26.0F, 128.0F, 17.0F);
    const float fraction = player.max_health > 0 ?
        std::clamp(static_cast<float>(player.health) /
                   static_cast<float>(player.max_health), 0.0F, 1.0F) : 0.0F;
    SDL_FRect fill{17.0F, height - 30.0F, 122.0F * fraction, 14.0F};
    SDL_SetRenderDrawColor(renderer, 183, 42, 39, 230);
    SDL_RenderFillRect(renderer, &fill);
    char health[48];
    std::snprintf(health, sizeof(health), "HP %d / %d", player.health, player.max_health);
    ui_text(renderer, 20.0F, height - 25.0F, health);

    if (player.owner >= 0 && player.owner < 4) {
        char money[32];
        std::snprintf(money, sizeof(money), "GOLD %d", game.run.coins[static_cast<std::size_t>(player.owner)]);
        small_ui_text(renderer, 20, height - 43, money, 218, 179, 97);
    }
    draw_player_status(renderer, graphics, game, player, 17, height - 48);
    const Item& held = *player.inventory.held();
    if (quiet) return;
    if (held.kind != ItemKind::None) {
        if (compact_details)
            draw_compact_item_details(renderer, graphics, held,
                                      width - 194.0F, height - 53.0F, 180.0F, "SELECTED");
        else
            draw_item_details(renderer, graphics, player, held,
                              width - 194.0F, height - 232.0F, 180.0F, 208.0F, "SELECTED");
    }
    const Entity* ground = nullptr;
    for (const Entity& entity : game.entities) {
        if (entity.kind != EntityKind::GroundItem ||
            entity.ground_item.kind == ItemKind::None) continue;
        if (pointer.inside && entity.cell == pointer.cell) { ground = &entity; break; }
        if (ground == nullptr && entity.cell == player.cell) ground = &entity;
    }
    const GroundAction action = ground_action(game, player);
    if (action != GroundAction::None)
        draw_action_hint(renderer, width * .5F - 90, height - 74, Action::Pickup,
            action == GroundAction::Drop ? "DROP HELD" : action == GroundAction::Swap ? "SWAP HELD" :
            action == GroundAction::Blocked ? "PACK FULL" : "PICK UP");
    if (ground == nullptr) return;
    const char* label = "GROUND";
    if (compact_details)
        draw_compact_item_details(renderer, graphics, ground->ground_item,
                                  width * .5F - 90.0F, height - 53.0F, 180.0F, label);
    else
        draw_item_details(renderer, graphics, player, ground->ground_item,
                          width * .5F - 90.0F, height - 232.0F, 180.0F, 208.0F, label);
}
