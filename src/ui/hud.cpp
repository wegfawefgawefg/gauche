#include "presentation.hpp"

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

const char* item_effect(ItemKind kind) {
    switch (kind) {
    case ItemKind::Wall: return "...a wall";
    case ItemKind::Medkit: return "first aid, second aid, cool-aid";
    case ItemKind::Bandage: return "a bandage to stop the bleeding";
    case ItemKind::Bandaid: return "a bandaid to stop the bleeding";
    case ItemKind::Fist: return "your fist";
    case ItemKind::ConductorHat: return "choo choo";
    case ItemKind::Stick: return "Heavy swing";
    case ItemKind::Pickaxe: return "Break rock";
    case ItemKind::Buckler: return "Block and shove";
    case ItemKind::Bomb: return "Blast radius 2";
    case ItemKind::SleepMeds: return "Put target to sleep";
    case ItemKind::BearTrap: case ItemKind::Mine: return "Place on ground";
    case ItemKind::RawMeat: case ItemKind::CookedMeat: return "Restore health";
    case ItemKind::Ammo: return "Stock your guns";
    default: return "Aim and fire";
    }
}

void description(SDL_Renderer* renderer, float x, float y, std::string_view words) {
    for (int line = 0; line < 2 && !words.empty(); ++line) {
        std::size_t length = std::min<std::size_t>(13, words.size());
        if (length < words.size()) {
            const std::size_t gap = words.rfind(' ', length);
            if (gap != std::string_view::npos && gap > 0) length = gap;
        }
        const std::string chunk{words.substr(0, length)};
        ui_text(renderer, x, y + static_cast<float>(line) * 10.0F, chunk.c_str());
        words.remove_prefix(length);
        while (!words.empty() && words.front() == ' ') words.remove_prefix(1);
    }
}

bool weapon(ItemKind kind) {
    switch (kind) {
    case ItemKind::Pistol: case ItemKind::Musket: case ItemKind::Bow:
    case ItemKind::RocketLauncher: case ItemKind::Shotgun: case ItemKind::SMG:
        return true;
    default: return false;
    }
}

void item_panel(SDL_Renderer* renderer, const GameGraphics& graphics,
                const Item& item, float x, float y, const char* label) {
    panel(renderer, x, y, 120.0F, 76.0F);
    SDL_FRect banner{x - 4.0F, y - 4.0F, 124.0F, 14.0F};
    SDL_SetRenderDrawColor(renderer, 113, 40, 38, 240);
    SDL_RenderFillRect(renderer, &banner);
    ui_text(renderer, x + 3.0F, y - 1.0F, label);
    SDL_FRect icon{x + 5.0F, y + 12.0F, 17.0F, 17.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item.kind)), nullptr, &icon);
    char name[14];
    std::snprintf(name, sizeof(name), "%.11s", item_name(item.kind));
    ui_text(renderer, x + 25.0F, y + 17.0F, name);
    description(renderer, x + 5.0F, y + 32.0F, item_effect(item.kind));
    char line[64];
    const ItemRange range = item_range(item.kind);
    if (range.maximum > 0) {
        std::snprintf(line, sizeof(line), "RANGE %d-%d", range.minimum, range.maximum);
        ui_text(renderer, x + 5.0F, y + 53.0F, line);
    }
    if (weapon(item.kind))
        std::snprintf(line, sizeof(line), "AMMO %d / %d", item.loaded, item.spare);
    else if (item.kind == ItemKind::Buckler)
        std::snprintf(line, sizeof(line), "SHIELD %d", item.durability);
    else
        std::snprintf(line, sizeof(line), "COUNT %d", item.count);
    ui_text(renderer, x + 5.0F, y + 65.0F, line);
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
        item_panel(renderer, graphics, held, 510.0F, 274.0F, "SELECTED");
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::GroundItem && entity.cell == player.cell &&
            entity.ground_item.kind != ItemKind::None) {
            item_panel(renderer, graphics, entity.ground_item, 375.0F, 274.0F, "E PICK UP");
            break;
        }
    }
}
