#include "presentation.hpp"
#include "item_details.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

namespace {

void ui_text(SDL_Renderer* renderer, float x, float y, const char* value) {
    SDL_SetRenderDrawColor(renderer, 235, 230, 214, 255);
    SDL_RenderDebugText(renderer, x, y, value);
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
              const Game& game, const Entity& player) {
    // The inventory stays clear of the player, with the selected row protruding.
    for (int index = 0; index < quick_slots; ++index) {
        const bool selected = index == player.inventory.selected;
        const float x = selected ? 25.0F : 17.0F;
        const float y = 58.0F + static_cast<float>(index) * 20.0F;
        panel(renderer, x, y, 104.0F, 18.0F, selected);
        char number[2]{static_cast<char>('1' + index), '\0'};
        ui_text(renderer, 4.0F, y + 5.0F, number);
        const Item& item = player.inventory.slots[static_cast<std::size_t>(index)];
        if (item.kind != ItemKind::None) {
            SDL_FRect icon{x + 3.0F, y + 1.0F, 15.0F, 15.0F};
            SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item.kind)),
                              nullptr, &icon);
            char name[10];
            std::snprintf(name, sizeof(name), item.count > 1 ? "%.6s" : "%.9s",
                          item_name(item.kind));
            ui_text(renderer, x + 21.0F, y + 4.0F, name);
            if (item.count > 1) {
                char count[16];
                std::snprintf(count, sizeof(count), "x%d", item.count);
                const float count_x = x + 101.0F -
                    static_cast<float>(std::char_traits<char>::length(count)) * 8.0F;
                ui_text(renderer, count_x, y + 4.0F, count);
            }
        }
        if (selected) {
            SDL_FRect arrow{x - 14.0F, y + 3.0F, 11.0F, 11.0F};
            SDL_RenderTexture(renderer, texture_for(graphics, Sprite::SelectedArrow),
                              nullptr, &arrow);
        }
    }

    // Rust's offset red bar is compacted to the half-size render target.
    panel(renderer, 14.0F, 334.0F, 128.0F, 17.0F);
    const float fraction = player.max_health > 0 ?
        std::clamp(static_cast<float>(player.health) /
                   static_cast<float>(player.max_health), 0.0F, 1.0F) : 0.0F;
    SDL_FRect fill{17.0F, 330.0F, 122.0F * fraction, 14.0F};
    SDL_SetRenderDrawColor(renderer, 183, 42, 39, 230);
    SDL_RenderFillRect(renderer, &fill);
    char health[48];
    std::snprintf(health, sizeof(health), "HP %d / %d", player.health, player.max_health);
    ui_text(renderer, 20.0F, 335.0F, health);

    const Item& held = *player.inventory.held();
    if (held.kind != ItemKind::None)
        draw_item_details(renderer, graphics, player, held,
                          446.0F, 164.0F, 180.0F, 172.0F, "SELECTED");
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::GroundItem && entity.cell == player.cell &&
            entity.ground_item.kind != ItemKind::None) {
            draw_item_details(renderer, graphics, player, entity.ground_item,
                              255.0F, 164.0F, 180.0F, 172.0F, "E PICK UP");
            break;
        }
    }
}
