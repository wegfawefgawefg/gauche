#include "item_details.hpp"
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
    switch (kind) {
    case ItemKind::Wall: return "Build a wall on nearby ground. It blocks paths and fire.";
    case ItemKind::Medkit: return "Restore up to 100 health. One use consumes one kit.";
    case ItemKind::Bandage: return "Restore 10 health. Quick, modest field medicine.";
    case ItemKind::Bandaid: return "Restore 1 health when there is time to spare.";
    case ItemKind::Fist: return "A direct punch into the next tile. Always available.";
    case ItemKind::ConductorHat: return "Lay track and call a train that tears through walls.";
    case ItemKind::Buckler: return "Block a hit, then shove the actor in front of you.";
    case ItemKind::Pistol: return "A reliable short-cooldown shot along a straight line.";
    case ItemKind::Musket: return "A heavy shot that can chip solid walls.";
    case ItemKind::Bow: return "A long straight shot with a single-arrow magazine.";
    case ItemKind::RocketLauncher: return "A rocket travels straight, then blasts the impact area.";
    case ItemKind::Ammo: return "Supply every gun separately, including the held weapon.";
    case ItemKind::Bomb: return "Throw a bomb a few tiles; its blast hits a wide area.";
    case ItemKind::SleepMeds: return "Put a nearby target to sleep for a short time.";
    case ItemKind::Stick: return "A sturdy one-tile strike with more force than a fist.";
    case ItemKind::Shotgun: return "Powerful close-range shot with a slow recovery.";
    case ItemKind::SMG: return "Rapid straight shots with a large magazine.";
    case ItemKind::BearTrap: return "Open the jaws first. Then place it; a victim takes 100 damage.";
    case ItemKind::Mine: return "Place an explosive trap on the next clear tile.";
    case ItemKind::Pickaxe: return "Strike the next tile and break weak rock quickly.";
    case ItemKind::RawMeat: return "Eat for 4 health, or cook it at a campfire.";
    case ItemKind::CookedMeat: return "Eat for 18 health after cooking it at a fire.";
    case ItemKind::None: return "Empty slot.";
    }
    return "";
}

void draw_item_details(SDL_Renderer* renderer, const GameGraphics& graphics,
                       const Entity& player, const Item& item, float x, float y,
                       float width, float height, const char* label) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    plate(renderer, x + 6.0F, y + 7.0F, width, height,
          {0.01F, 0.01F, 0.01F, 0.72F});
    plate(renderer, x, y, width, height,
          {0.075F, 0.085F, 0.09F, 0.96F});
    plate(renderer, x - 6.0F, y - 8.0F, width + 8.0F, 18.0F,
          {0.49F, 0.15F, 0.14F, 0.98F});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    text(renderer, x + 3.0F, y - 5.0F, label);
    if (item.kind == ItemKind::None) return;
    SDL_FRect icon{x + 9.0F, y + 22.0F, 24.0F, 24.0F};
    SDL_RenderTexture(renderer, texture_for(graphics,
        item.kind == ItemKind::BearTrap && item.opened ?
        Sprite::BearTrapOpen : item_sprite(item.kind)), nullptr, &icon);
    text(renderer, x + 39.0F, y + 22.0F, item_name(item.kind));
    char line[80];
    std::snprintf(line, sizeof(line), "x%d  %s", item.count,
                  item.cooldown > 0 ? "COOLING" : "READY");
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
    if (pattern.damage > 0)
        std::snprintf(line, sizeof(line), "DAMAGE %d", pattern.damage);
    else if (pattern.heal > 0)
        std::snprintf(line, sizeof(line), "HEAL +%d   HP %d/%d", pattern.heal,
                      player.health, player.max_health);
    else std::snprintf(line, sizeof(line), "%s", item.opened ? "OPEN" : "UTILITY");
    text(renderer, x + 10.0F, y + 96.0F, line);
    std::snprintf(line, sizeof(line), "COOLDOWN %.2f / %.2fs",
                  static_cast<double>(item.cooldown) / 60.0,
                  static_cast<double>(pattern.cooldown) / 60.0);
    text(renderer, x + 10.0F, y + 107.0F, line, 188, 187, 176);
    if (item_is_gun(item.kind))
        std::snprintf(line, sizeof(line), "MAG %d / %d   SPARE %d",
                      item.loaded, item_meter_capacity(item), item.spare);
    else if (item.max_durability > 0)
        std::snprintf(line, sizeof(line), "CONDITION %d / %d",
                      item.durability, item.max_durability);
    else if (item.max_uses > 0)
        std::snprintf(line, sizeof(line), "USES %d / %d",
                      item.uses, item.max_uses);
    else std::snprintf(line, sizeof(line), "STACK %d", item.count);
    text(renderer, x + 10.0F, y + 118.0F, line, 194, 192, 180);
    std::snprintf(line, sizeof(line), "RANGE %d-%d", pattern.minimum, pattern.maximum);
    text(renderer, x + 10.0F, y + 129.0F, line, 194, 192, 180);
    if (height >= 165.0F) {
        text(renderer, x + 10.0F, y + 140.0F, "PATTERN", 185, 185, 172);
        draw_pattern_diagram(renderer, item, x + 10.0F, y + 148.0F, width);
    }
}
