#include "item_meter.hpp"
#include "../items/catalog.hpp"
#include "../item_attribute.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <cstdio>

std::string item_cooldown_text(const Item& item) {
    const int total = item_pattern(item).cooldown;
    char result[32];
    std::snprintf(result, sizeof(result), "%.1f/%.1f",
                  static_cast<double>(item.cooldown) / 60.0,
                  static_cast<double>(total) / 60.0);
    return result;
}

std::string item_state_text(const Item& item, bool compact) {
    char result[32];
    if (item.max_durability > 0)
        std::snprintf(result, sizeof(result), compact ? "%d/%d" : "COND %d/%d",
                      item.durability, item.max_durability);
    else if (item.max_uses > 0)
        std::snprintf(result, sizeof(result), compact ? "%d/%d" : "U %d/%d",
                      item.uses, item.max_uses);
    else if (item_is_gun(item.kind))
        if (compact)
            std::snprintf(result, sizeof(result), "%d+%d", item.loaded, item.spare);
        else
            std::snprintf(result, sizeof(result), "MAG %d/%d +%d",
                          item.loaded, item_meter_capacity(item), item.spare);
    else if (item.max_count > 1)
        std::snprintf(result, sizeof(result), compact ? "x%d" : "STACK %d/%d",
                      item.count, item.max_count);
    else if (item.kind != ItemKind::None)
        std::snprintf(result, sizeof(result), "x%d", item.count);
    else return {};
    return result;
}

int item_meter_capacity(const Item& item) {
    if (item.max_durability > 0) return item.max_durability;
    if (item.max_uses > 0) return item.max_uses;
    if (const RegionalItem* spec = regional_item(item.kind); spec != nullptr && spec->magazine > 0)
        return spec->magazine;
    switch (item.kind) {
    case ItemKind::Pistol: return 12;
    case ItemKind::Shotgun: return 6;
    case ItemKind::SMG: return 30;
    case ItemKind::Musket: case ItemKind::Bow:
    case ItemKind::RocketLauncher: return 1;
    default: return item.max_count;
    }
}

int item_meter_current(const Item& item) {
    if (item.max_durability > 0) return item.durability;
    if (item.max_uses > 0) return item.uses;
    if (item_is_gun(item.kind)) return item.loaded;
    return item.count;
}

void draw_item_meter(SDL_Renderer* renderer, float x, float y,
                     float width, float height, int current, int maximum,
                     SDL_Color color) {
    SDL_FRect base{x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 45, 49, 44, 230);
    SDL_RenderFillRect(renderer, &base);
    if (maximum <= 0) return;
    const float amount = std::clamp(static_cast<float>(current) /
                                    static_cast<float>(maximum), 0.0F, 1.0F);
    base.w *= amount;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 240);
    SDL_RenderFillRect(renderer, &base);
}
