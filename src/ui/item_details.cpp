#include "../items/fire_render.hpp"
#include "item_details.hpp"
#include "../items/action.hpp"
#include "../items/catalog.hpp"
#include "../items/woodland_tools.hpp"
#include "../item_pattern.hpp"
#include "../item_attribute.hpp"
#include "pattern_diagram.hpp"
#include "item_meter.hpp"
#include "text.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

namespace {

void plate(SDL_Renderer* renderer, float x, float y, float width, float height,
           SDL_FColor color) {
    const SDL_Vertex vertices[4]{
        {{x + 4.0F, y}, color, {}},
        {{x + width + 4.0F, y}, color, {}},
        {{x + width, y + height}, color, {}},
        {{x, y + height}, color, {}},
    };
    constexpr int indices[]{0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
}

void text(SDL_Renderer* renderer, float x, float y, std::string_view words,
          std::uint8_t red = 235, std::uint8_t green = 230,
          std::uint8_t blue = 214) {
    small_ui_text(renderer, x, y, words, red, green, blue);
}

void wrapped(SDL_Renderer* renderer, float x, float y, int columns,
             int lines, std::string_view words) {
    for (int line = 0; line < lines && !words.empty(); ++line) {
        std::size_t count = std::min(words.size(), static_cast<std::size_t>(columns));
        if (count < words.size()) {
            const std::size_t gap = words.rfind(' ', count);
            if (gap != std::string_view::npos && gap > 0) count = gap;
        }
        text(renderer, x, y + static_cast<float>(line) * 9.0F,
             words.substr(0, count), 194, 192, 180);
        words.remove_prefix(count);
        while (!words.empty() && words.front() == ' ') words.remove_prefix(1);
    }
}

} // namespace

const char* item_description(ItemKind kind) {
    if (const RegionalItem* spec = regional_item(kind)) return spec->description;
    switch (kind) {
    case ItemKind::Wall: return "Build a wall on nearby ground. It blocks paths and fire.";
    case ItemKind::Medkit: return "Restore up to 100 health. One use consumes one kit.";
    case ItemKind::Bandage: return "Restore 10 health. Quick, modest field medicine.";
    case ItemKind::Bandaid: return "Restore 1 health when there is time to spare.";
    case ItemKind::Fist: return "A direct punch into the next tile. Always available.";
    case ItemKind::ConductorHat: return "Lay track and call a train that tears through walls.";
    case ItemKind::Buckler: return "Block a hit, then shove the actor in front of you.";
    case ItemKind::Pistol: return "A reliable short-cooldown shot along a straight line.";
    case ItemKind::Musket: return "A loud, powerful single shot. Slow to reload.";
    case ItemKind::Bow: return "Hold to draw; release an arrow. No reload. Damage arrives with the arrow.";
    case ItemKind::RocketLauncher: return "A rocket travels straight, then blasts the impact area.";
    case ItemKind::Ammo: return "Supply each gun and bow separately, including the held weapon.";
    case ItemKind::Bomb: return "Throw forward. A 2.5s fuse starts on use, then it explodes. Get clear!";
    case ItemKind::SleepMeds: return "Put a nearby target to sleep for a short time.";
    case ItemKind::Stick: return "Hit harder than a fist. Light at a campfire for 30s of fire strikes; water puts it out.";
    case ItemKind::Shotgun: return "Powerful close-range shot with a slow recovery.";
    case ItemKind::SMG: return "Rapid straight shots with a large magazine.";
    case ItemKind::BearTrap: return "Open the jaws first. Then place it; a victim takes 100 damage.";
    case ItemKind::Mine: return "Place an explosive trap on the next clear tile.";
    case ItemKind::Pickaxe: return "Strike the next tile and break weak rock quickly.";
    case ItemKind::RawMeat: return "Eat for 4 health, or cook it at a campfire.";
    case ItemKind::CookedMeat: return "Eat for 18 health after cooking it at a fire.";
    case ItemKind::None: return "Empty slot.";
    default: break;
    }
    return "";
}

void draw_item_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                       const Entity& player, const Item& item, float x, float y,
                       float width, float height, const char* label, bool highlight) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x + 6.0F, y + 7.0F, width, height,
          {0.01F, 0.01F, 0.01F, 0.72F});
    plate(renderer, x, y, width, height,
          {0.075F, 0.085F, 0.09F, 0.96F});
    draw_item_banner(renderer, x, y, width, label, highlight);
    if (item.kind == ItemKind::None) return;
    SDL_FRect icon{x + 9.0F, y + 22.0F, 24.0F, 24.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item)), nullptr, &icon);
    draw_item_flame(renderer, graphics, item, icon, {1, 0}, static_cast<std::uint64_t>(item.flame_ticks));
    text(renderer, x + 39.0F, y + 22.0F, item.flame_ticks > 0 ? "Lit Stick" : item_name(item.kind));
    char line[80];
    if (item_stackable(item))
        std::snprintf(line, sizeof(line), "x%d  %s", item.count, item.cooldown > 0 ? "COOLING" : "READY");
    else std::snprintf(line, sizeof(line), "%s", item.cooldown > 0 ? "COOLING" : "READY");
    if (item.flight.slot >= 0) std::snprintf(line, sizeof(line), "IN FLIGHT");
    if (item.attribute != ItemAttribute::None) {
        text(renderer, x + 39.0F, y + 34.0F,
             item_attribute_name(item.attribute), 218, 169, 94);
        text(renderer, x + 10.0F, y + 48.0F,
             item_attribute_effect(item.attribute), 218, 169, 94);
    } else text(renderer, x + 39.0F, y + 34.0F, line, 186, 189, 174);
    wrapped(renderer, x + 10.0F, y + 59.0F,
            static_cast<int>((width - 20.0F) / 6.0F), 4,
            item_description(item.kind));
    const ItemPattern pattern = item_pattern(item);
    if (pattern.damage > 0 && item.dig_power > 0)
        std::snprintf(line, sizeof(line), "DAMAGE %d  DIG %d", pattern.damage, item.dig_power);
    else if (pattern.damage > 0)
        std::snprintf(line, sizeof(line), "DAMAGE %d", pattern.damage);
    else if (pattern.heal > 0)
        std::snprintf(line, sizeof(line), "HEAL +%d   HP %d/%d", pattern.heal,
                      player.health, player.max_health);
    else std::snprintf(line, sizeof(line), "%s", item.opened ? "OPEN" : "UTILITY");
    if (item.kind == ItemKind::ResinGlue) {
        const int repair = resin_repair_slot(player.inventory);
        if (repair < 0) std::snprintf(line, sizeof(line), "NOTHING NEEDS REPAIR");
        else {
            const Item& target = player.inventory.slots[static_cast<std::size_t>(repair)];
            std::snprintf(line, sizeof(line), "%s %d -> %d", item_name(target.kind),
                target.durability, target.max_durability);
        }
    }
    if (item.kind == ItemKind::HerbBag)
        std::snprintf(line, sizeof(line), "REGEN +%d OVER %.1fs", pattern.heal,
            static_cast<double>(pattern.heal) / 3);
    if (item.kind == ItemKind::BitterRoot) {
        if (player.health <= 3) std::snprintf(line, sizeof(line), "NEEDS MORE THAN 3 HP");
        else std::snprintf(line, sizeof(line), "HP %d -> %d | GUARD 10s", player.health, player.health - 3);
    }
    if (item.kind == ItemKind::Chili) {
        Entity faster = player;
        faster.vitals.haste = 240;
        std::snprintf(line, sizeof(line), "STEP %d -> %d TICKS", movement_beat(player, player.move_interval),
            movement_beat(faster, faster.move_interval));
    }
    text(renderer, x + 10.0F, y + 96.0F, line);
    if (item.flight.slot >= 0) std::snprintf(line, sizeof(line), "IN FLIGHT - SLOT RESERVED");
    else std::snprintf(line, sizeof(line), "COOLDOWN %.2f / %.2fs",
                  static_cast<double>(item.cooldown) / 60.0,
                  static_cast<double>(pattern.cooldown) / 60.0);
    text(renderer, x + 10.0F, y + 107.0F, line, 188, 187, 176);
    if (item.kind == ItemKind::Bow)
        std::snprintf(line, sizeof(line), "QUIVER %d ARROWS", item.loaded);
    else if (item_is_gun(item.kind))
        std::snprintf(line, sizeof(line), "MAG %d / %d   SPARE %d",
                      item.loaded, item_meter_capacity(item), item.spare);
    else if (item.max_durability > 0)
        std::snprintf(line, sizeof(line), "CONDITION %d / %d",
                      item.durability, item.max_durability);
    else if (item.max_uses > 0)
        std::snprintf(line, sizeof(line), "USES %d / %d",
                      item.uses, item.max_uses);
    else if (item_stackable(item))
        std::snprintf(line, sizeof(line), "STACK %d / %d   %s",
                      item.count, item.max_count,
                      item.consume_on_use ? "CONSUMES" : "PERSISTS");
    else std::snprintf(line, sizeof(line), "%s",
                       item.consume_on_use ? "ONE USE - CONSUMES" : "PERSISTENT");
    text(renderer, x + 10.0F, y + 118.0F, line, 194, 192, 180);
    if (item.flame_ticks > 0) {
        std::snprintf(line, sizeof(line), "FIRE %.1fs  BURN 20 / 5s", static_cast<double>(item.flame_ticks)/60);
        text(renderer, x + 10.0F, y + 129.0F, line, 235, 167, 80);
    } else text(renderer, x + 10.0F, y + 129.0F, item_stackable(item) ? "STACKABLE" : "NOT STACKABLE", 162, 171, 159);
    if (item_windup(item) > 0)
        std::snprintf(line, sizeof(line), "WINDUP %.2fs  RANGE %d-%d",
            static_cast<double>(item_windup(item)) / 60.0, pattern.minimum, pattern.maximum);
    else if (item.dig_power > 0)
        std::snprintf(line, sizeof(line), "RANGE %d-%d   DIG %d", pattern.minimum,
                      pattern.maximum, item.dig_power);
    else std::snprintf(line, sizeof(line), "RANGE %d-%d", pattern.minimum, pattern.maximum);
    text(renderer, x + 10.0F, y + 140.0F, line, 194, 192, 180);
    if (height >= 176.0F) {
        text(renderer, x + 10.0F, y + 151.0F, "PATTERN", 185, 185, 172);
        draw_pattern_diagram(renderer, item, x + 10.0F, y + 159.0F,
                             width - 20.0F, height - 169.0F);
    }
}

void draw_compact_item_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                               const Item& item, float x, float y,
                               float width, const char* label) {
    if (item.kind == ItemKind::None) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x + 4.0F, y + 4.0F, width, 31.0F,
          {0.01F, 0.01F, 0.01F, 0.72F});
    plate(renderer, x, y, width, 31.0F,
          {0.075F, 0.085F, 0.09F, 0.96F});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_FRect icon{x + 5.0F, y + 7.0F, 17.0F, 17.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item)), nullptr, &icon);
    draw_item_flame(renderer, graphics, item, icon, {1, 0}, static_cast<std::uint64_t>(item.flame_ticks));
    if (item.flame_ticks > 0) {
        char status[64];
        std::snprintf(status, sizeof(status), "%s  FIRE %ds", label, (item.flame_ticks+59)/60);
        text(renderer, x + 28.0F, y + 3.0F, status, 235, 167, 80);
    } else text(renderer, x + 28.0F, y + 3.0F, label, 178, 164, 151);
    text(renderer, x + 28.0F, y + 14.0F, item.flame_ticks > 0 ? "Lit Stick" : item_name(item.kind));
    text(renderer, x + width - 54.0F, y + 14.0F,
         item_state_text(item, true), 200, 207, 189);
    text(renderer, x + width - 54.0F, y + 23.0F,
         item_cooldown_text(item), 217, 183, 128);
}

void draw_item_banner(SDL_Renderer* renderer, float x, float y, float width,
                      const char* label, bool highlight) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x-6, y-8, width+8, 18, highlight ?
        SDL_FColor{.56F, .16F, .14F, .98F} : SDL_FColor{.14F, .17F, .15F, .98F});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    text(renderer, x+3, y-5, label);
}
