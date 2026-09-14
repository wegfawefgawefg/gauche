#include "item_details.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
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
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    const std::string copy{words};
    SDL_RenderDebugText(renderer, x, y, copy.c_str());
}

void wrapped(SDL_Renderer* renderer, float x, float y, int columns,
             int lines, std::string_view words) {
    for (int line = 0; line < lines && !words.empty(); ++line) {
        std::size_t count = std::min(words.size(), static_cast<std::size_t>(columns));
        if (count < words.size()) {
            const std::size_t gap = words.rfind(' ', count);
            if (gap != std::string_view::npos && gap > 0) count = gap;
        }
        text(renderer, x, y + static_cast<float>(line) * 11.0F,
             words.substr(0, count), 194, 192, 180);
        words.remove_prefix(count);
        while (!words.empty() && words.front() == ' ') words.remove_prefix(1);
    }
}

void pattern_cell(SDL_Renderer* renderer, float x, float y, float side,
                  PatternEffect effect, bool travel) {
    const std::uint8_t red = effect == PatternEffect::Damage ? 222 :
                             (effect == PatternEffect::Heal ? 87 : 212);
    const std::uint8_t green = effect == PatternEffect::Damage ? 73 :
                               (effect == PatternEffect::Heal ? 206 : 210);
    const std::uint8_t blue = effect == PatternEffect::Damage ? 57 :
                              (effect == PatternEffect::Heal ? 112 : 193);
    SDL_FRect cell{x, y, side - 1.0F, side - 1.0F};
    SDL_SetRenderDrawColor(renderer, red, green, blue, travel ? 85 : 230);
    if (travel) SDL_RenderRect(renderer, &cell);
    else SDL_RenderFillRect(renderer, &cell);
}

void pattern_diagram(SDL_Renderer* renderer, ItemKind kind,
                     float x, float y, float width) {
    const ItemPattern pattern = item_pattern(kind);
    if (pattern.effect == PatternEffect::None) return;
    const float side = pattern.maximum > 10 ? 6.0F : 8.0F;
    const float left = x + 12.0F;
    const float middle = y + 16.0F;
    text(renderer, left, middle - 3.0F, "P", 235, 230, 214);
    if (pattern.minimum == 0 && pattern.maximum == 0) {
        pattern_cell(renderer, left + side + 3.0F, middle, side,
                     pattern.effect, false);
        return;
    }
    const int reach = std::min(pattern.maximum, static_cast<int>((width - 35.0F) / side));
    for (int step = 1; step <= reach; ++step)
        pattern_cell(renderer, left + 12.0F + static_cast<float>(step - 1) * side,
                     middle, side, pattern.effect,
                     pattern.blast_radius > 0 && (pattern.ray || step < reach));
    if (pattern.blast_radius == 0) return;
    const float impact_x = left + 12.0F + static_cast<float>(reach - 1) * side;
    for (int dy = -pattern.blast_radius; dy <= pattern.blast_radius; ++dy)
        for (int dx = -pattern.blast_radius; dx <= pattern.blast_radius; ++dx)
            if (std::abs(dx) + std::abs(dy) <= pattern.blast_radius)
                pattern_cell(renderer, impact_x + static_cast<float>(dx) * side,
                             middle + static_cast<float>(dy) * side,
                             side, pattern.effect, false);
}

int heal_amount(ItemKind kind) {
    switch (kind) {
    case ItemKind::Medkit: return 100;
    case ItemKind::Bandage: return 10;
    case ItemKind::Bandaid: return 1;
    case ItemKind::RawMeat: return 4;
    case ItemKind::CookedMeat: return 18;
    default: return 0;
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
    case ItemKind::BearTrap: return "Place a trap on the next clear tile.";
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
    SDL_FRect icon{x + 9.0F, y + 22.0F, 30.0F, 30.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item.kind)), nullptr, &icon);
    text(renderer, x + 46.0F, y + 23.0F, item_name(item.kind));
    char line[80];
    std::snprintf(line, sizeof(line), "x%d  %s", item.count,
                  item.cooldown > 0 ? "COOLING" : "READY");
    text(renderer, x + 46.0F, y + 38.0F, line, 186, 189, 174);
    wrapped(renderer, x + 10.0F, y + 61.0F,
            static_cast<int>((width - 20.0F) / 8.0F), 3,
            item_description(item.kind));
    const ItemPattern pattern = item_pattern(item.kind);
    if (pattern.damage > 0)
        std::snprintf(line, sizeof(line), "DMG %d  CD %.2fs",
                      pattern.damage, static_cast<double>(pattern.cooldown) / 60.0);
    else if (heal_amount(item.kind) > 0)
        std::snprintf(line, sizeof(line), "HEAL +%d  HP %d/%d", heal_amount(item.kind),
                      player.health, player.max_health);
    else std::snprintf(line, sizeof(line), "CD %.2fs",
                       static_cast<double>(pattern.cooldown) / 60.0);
    text(renderer, x + 10.0F, y + 101.0F, line);
    if (item.loaded > 0 || item.spare > 0)
        std::snprintf(line, sizeof(line), "AMMO %d / %d",
                      item.loaded, item.spare);
    else if (item.durability > 0)
        std::snprintf(line, sizeof(line), "SHIELD %d", item.durability);
    else std::snprintf(line, sizeof(line), "RANGE %d-%d",
                       pattern.minimum, pattern.maximum);
    text(renderer, x + 10.0F, y + 116.0F, line, 194, 192, 180);
    if (height >= 165.0F) {
        text(renderer, x + 10.0F, y + 136.0F, "PATTERN", 185, 185, 172);
        pattern_diagram(renderer, item.kind, x, y + 142.0F, width);
    }
}
