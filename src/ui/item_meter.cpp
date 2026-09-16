#include "../items/heated_water.hpp"
#include "../items/pocket_pump.hpp"
#include "item_meter.hpp"
#include "../items/effigy_mask.hpp"
#include "text.hpp"
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
    if (item.flight.slot >= 0) return item.kind == ItemKind::HarpoonGun ? "LINE OUT" : compact ? "OUT" : "IN FLIGHT";
    char result[32]{};
    if (item.kind==ItemKind::LunchTin) {
        if (!item.loaded) return compact ? "EMPTY" : "EMPTY | THROW TO LURE";
        std::snprintf(result,sizeof(result),compact ? "%d/2" : "MEALS %d/2",item.loaded);
    } else if (item.kind==ItemKind::GlowSlag) {
        if (!item.loaded) return "COLD";
        std::snprintf(result,sizeof(result),compact ? "%ds" : "GLOW %ds",(item.loaded+59)/60);
    } else if (item.kind==ItemKind::NozzleElbow) {
        if (compact) std::snprintf(result,sizeof(result),"%dHP %s",item.durability,item.loaded ? "L" : "R");
        else std::snprintf(result,sizeof(result),"FITTING %d/%d HP",item.durability,item.max_durability);
    } else if (item.kind==ItemKind::PocketPump) {
        if (item.spare==0) return "EMPTY";
        const int percent=(item.spare*100+pump_capacity-1)/pump_capacity;
        if (compact) std::snprintf(result,sizeof(result),"%c%d%%",pump_contents(item)[0],percent);
        else std::snprintf(result,sizeof(result),"TANK %s %d%%",pump_contents(item),percent);
    } else if (item.kind==ItemKind::IceAnchor) {
        if (compact) std::snprintf(result,sizeof(result),"%dHP %s",item.durability,item.anchor.slot>=0 ? "SET" : "");
        else std::snprintf(result,sizeof(result),"POINT %d/%d HP",item.durability,item.max_durability);
    } else if (item.kind==ItemKind::EffigyMask) {
        std::snprintf(result,sizeof(result),compact ? "%.1fs" : "WEAR %.1fs LEFT",static_cast<double>(effigy_mask_ticks(item))/60);
    } else if (item.kind==ItemKind::HeatSiphon) {
        std::snprintf(result,sizeof(result),compact ? "%.1f/6" : "HEAT %.1f / 6 CHARGES",static_cast<double>(item.loaded)/300.0);
    } else if (item.kind==ItemKind::StormLantern) {
        std::snprintf(result,sizeof(result),"%ds %s",(item.loaded+59)/60,item.opened ? "LIT" : "SHUT");
    } else if (heated_water_item(item.kind)) {
        if (item.loaded == 0) return "EMPTY";
        if (item.loaded == 2) std::snprintf(result,sizeof(result),compact ? "HOT %ds" : "HOT | COOLS IN %ds",(item.spare+59)/60);
        else if (item.spare > 0) std::snprintf(result,sizeof(result),compact ? "H %d%%" : "HEATING %d%%",item.spare*100/90);
        else return compact ? "COLD" : "COLD WATER";
    } else if (item.kind == ItemKind::GroundingSpike) {
        if (item_stackable(item)) std::snprintf(result,sizeof(result),"x%d %dHP",item.count,item.durability);
        else std::snprintf(result,sizeof(result),"%d/%dHP",item.durability,item.max_durability);
    } else if (item.kind == ItemKind::CandleStub) {
        if (item_stackable(item)) std::snprintf(result, sizeof(result), "x%d %ds", item.count, (item.loaded+59)/60);
        else std::snprintf(result, sizeof(result), "%ds %d/%d", (item.loaded+59)/60, item.durability, item.max_durability);
    } else if (item.kind == ItemKind::SnowScoop)
        std::snprintf(result, sizeof(result), compact ? "%d/%d S%d" : "COND %d/%d SNOW %d", item.durability, item.max_durability, item.loaded);
    else if (item.kind == ItemKind::Bow)
        std::snprintf(result, sizeof(result), compact ? "%d" : "QUIVER %d ARROWS", item.loaded);
    else if (item.max_durability > 0)
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
    else if (item_stackable(item))
        std::snprintf(result, sizeof(result), compact ? "x%d" : "STACK %d/%d",
                      item.count, item.max_count);
    std::string state = result;
    if (!compact && item.muffled_uses > 0) {
        if (!state.empty()) state += " ";
        state += "QUIET ";
        state += std::to_string(item.muffled_uses);
    }
    return state;
}

int item_meter_capacity(const Item& item) {
    if (item.kind==ItemKind::LunchTin) return 2;
    if (item.kind==ItemKind::GlowSlag) return 1200;
    if (item.kind==ItemKind::PocketPump) return pump_capacity;
    if (item.kind==ItemKind::EffigyMask) return item.max_uses*60;
    if (item.kind==ItemKind::HeatSiphon) return 1800;
    if (item.kind==ItemKind::StormLantern) return 7200;
    if (heated_water_item(item.kind)) return item.loaded == 2 ? 1800 : 90;
    if (item.max_durability > 0) return item.max_durability;
    if (item.max_uses > 0) return item.max_uses;
    if (const RegionalItem* spec = regional_item(item.kind); spec != nullptr && spec->magazine > 0)
        return spec->magazine;
    switch (item.kind) {
    case ItemKind::Pistol: return 12;
    case ItemKind::Shotgun: return 6;
    case ItemKind::SMG: return 30;
    case ItemKind::Bow: return std::max(20, item.loaded);
    case ItemKind::Musket:
    case ItemKind::RocketLauncher: return 1;
    default: return item_stackable(item) ? item.max_count : 0;
    }
}

int item_meter_current(const Item& item) {
    if (item.kind==ItemKind::LunchTin) return item.loaded;
    if (item.kind==ItemKind::GlowSlag) return item.loaded;
    if (item.kind==ItemKind::PocketPump) return item.spare;
    if (item.kind==ItemKind::EffigyMask) return effigy_mask_ticks(item);
    if (item.kind==ItemKind::HeatSiphon) return item.loaded;
    if (item.kind==ItemKind::StormLantern) return item.loaded;
    if (heated_water_item(item.kind)) return item.spare;
    if (item.max_durability > 0) return item.durability;
    if (item.max_uses > 0) return item.uses;
    if (item_is_gun(item.kind)) return item.loaded;
    return item.count;
}

void draw_item_meter(SDL_Renderer* renderer, float x, float y,
                     float width, float height, int current, int maximum,
                     SDL_Color color) {
    if (maximum <= 0) return;
    SDL_FRect base{x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 45, 49, 44, 230);
    SDL_RenderFillRect(renderer, &base);
    const float amount = std::clamp(static_cast<float>(current) /
                                    static_cast<float>(maximum), 0.0F, 1.0F);
    base.w *= amount;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 240);
    SDL_RenderFillRect(renderer, &base);
}

void draw_muffled_count(SDL_Renderer* renderer, const Item& item, float x, float y) {
    if (item.muffled_uses == 0) return;
    const SDL_FRect badge{x, y, 14, 8};
    SDL_SetRenderDrawColor(renderer, 27, 42, 43, 255);
    SDL_RenderFillRect(renderer, &badge);
    const char label[]{'Q', static_cast<char>('0' + item.muffled_uses), '\0'};
    small_ui_text(renderer, x + 1, y, label, 162, 210, 202);
}
