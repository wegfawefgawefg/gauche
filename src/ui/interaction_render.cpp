#include "interaction.hpp"
#include "item_details.hpp"
#include "../item_pattern.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

namespace {

void text(SDL_Renderer* renderer, float x, float y, std::string_view value,
          std::uint8_t red = 235, std::uint8_t green = 230,
          std::uint8_t blue = 214) {
    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    const std::string copy{value};
    SDL_RenderDebugText(renderer, x, y, copy.c_str());
}

void shade(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 4, 7, 8, 215);
    const SDL_FRect full{0.0F, 0.0F, 640.0F, 360.0F};
    SDL_RenderFillRect(renderer, &full);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void frame(SDL_Renderer* renderer, float x, float y, float width, float height,
           bool selected) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_FRect shadow{x + 5.0F, y + 6.0F, width, height};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 190);
    SDL_RenderFillRect(renderer, &shadow);
    const SDL_Vertex vertices[4]{
        {{x + 4.0F, y}, {0.10F, 0.10F, 0.10F, 0.98F}, {}},
        {{x + width + 4.0F, y}, {0.10F, 0.10F, 0.10F, 0.98F}, {}},
        {{x + width, y + height}, {0.06F, 0.07F, 0.07F, 0.98F}, {}},
        {{x, y + height}, {0.06F, 0.07F, 0.07F, 0.98F}, {}},
    };
    constexpr int indices[]{0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
    SDL_SetRenderDrawColor(renderer, selected ? 202 : 90,
                           selected ? 151 : 86, selected ? 82 : 78, 210);
    const SDL_FPoint outline[5]{{x + 4.0F, y}, {x + width + 4.0F, y},
                                {x + width, y + height}, {x, y + height},
                                {x + 4.0F, y}};
    SDL_RenderLines(renderer, outline, 5);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

Sprite reward_icon(const Reward& reward) {
    switch (reward.kind) {
    case RewardKind::Item: return item_sprite(reward.item);
    case RewardKind::Health: return Sprite::Medkit;
    case RewardKind::Speed: return Sprite::PlayerFootprint;
    case RewardKind::Artifact:
        switch (reward.artifact) {
        case ArtifactKind::AllPiercing: return Sprite::Musket;
        case ArtifactKind::Reflector: return Sprite::Buckler;
        case ArtifactKind::Hearth: return Sprite::Campfire;
        case ArtifactKind::FleetFeet: return Sprite::PlayerFootprint;
        case ArtifactKind::None: break;
        }
    }
    return Sprite::Fist;
}

const char* reward_name(const Reward& reward) {
    switch (reward.kind) {
    case RewardKind::Item: return item_name(reward.item);
    case RewardKind::Health: return "Heartier";
    case RewardKind::Speed: return "Faster Steps";
    case RewardKind::Artifact:
        switch (reward.artifact) {
        case ArtifactKind::AllPiercing: return "All Piercing";
        case ArtifactKind::Reflector: return "Reflector";
        case ArtifactKind::Hearth: return "Hearth";
        case ArtifactKind::FleetFeet: return "Fleet Feet";
        case ArtifactKind::None: break;
        }
    }
    return "Unknown";
}

const char* reward_description(const Reward& reward) {
    switch (reward.kind) {
    case RewardKind::Item: return item_description(reward.item);
    case RewardKind::Health: return "Increase maximum health and heal by the same amount.";
    case RewardKind::Speed: return "Take each tile step sooner. Movement remains rectilinear.";
    case RewardKind::Artifact:
        switch (reward.artifact) {
        case ArtifactKind::AllPiercing:
            return "Shots pass through actors and keep traveling until a wall.";
        case ArtifactKind::Reflector:
            return "Some incoming damage bounces back at the attacker.";
        case ArtifactKind::Hearth:
            return "A small healing aura restores nearby friends over time.";
        case ArtifactKind::FleetFeet:
            return "Move between tiles faster for the rest of this run.";
        case ArtifactKind::None: break;
        }
    }
    return "";
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

void reward_card(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Entity& player, Reward reward, float x, float y,
                 float width, bool selected, const char* label) {
    if (reward.kind == RewardKind::Item) {
        if (selected) frame(renderer, x - 2.0F, y - 2.0F,
                            width + 4.0F, 218.0F, true);
        draw_item_details(renderer, graphics, player,
                          reward_item(reward),
                          x, y, width, 214.0F, label);
        return;
    }
    frame(renderer, x, y, width, 214.0F, selected);
    text(renderer, x + 8.0F, y + 13.0F, label, 206, 158, 92);
    SDL_FRect icon{x + 10.0F, y + 39.0F, 34.0F, 34.0F};
    SDL_RenderTexture(renderer, texture_for(graphics, reward_icon(reward)), nullptr, &icon);
    text(renderer, x + 51.0F, y + 47.0F, reward_name(reward));
    wrapped(renderer, x + 10.0F, y + 89.0F,
            static_cast<int>((width - 20.0F) / 8.0F), 4,
            reward_description(reward));
    char line[80];
    if (reward.kind == RewardKind::Health)
        std::snprintf(line, sizeof(line), "MAX HP %d -> %d", player.max_health,
                      player.max_health + reward.amount);
    else if (reward.kind == RewardKind::Speed ||
             (reward.kind == RewardKind::Artifact &&
              reward.artifact == ArtifactKind::FleetFeet)) {
        const int reduction = reward.kind == RewardKind::Speed ? reward.amount : 2;
        const int after = std::max(3, player.move_interval - reduction);
        std::snprintf(line, sizeof(line), "STEP %.2fs -> %.2fs",
                      static_cast<double>(player.move_interval) / 60.0,
                      static_cast<double>(after) / 60.0);
    } else if (reward.kind == RewardKind::Artifact &&
               has_artifact(player, reward.artifact))
        std::snprintf(line, sizeof(line), "ALREADY OWNED");
    else std::snprintf(line, sizeof(line), "PASSIVE ARTIFACT");
    text(renderer, x + 10.0F, y + 160.0F, line, 136, 213, 147);
}

void inventory_rows(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Entity& player, const InteractionUi& ui) {
    const float slide = ui.slide;
    const float x = 22.0F - (1.0F - slide) * 180.0F;
    text(renderer, x + 8.0F, 36.0F, "INVENTORY");
    for (int index = 0; index < quick_slots; ++index) {
        const float y = 62.0F + static_cast<float>(index) * 39.0F;
        frame(renderer, x + (index == ui.slot_focus ? 8.0F : 0.0F), y,
              154.0F, 31.0F, index == ui.slot_focus);
        char number[4];
        std::snprintf(number, sizeof(number), "%d", index + 1);
        text(renderer, x + 8.0F, y + 11.0F, number, 151, 149, 140);
        const Item& item = player.inventory.slots[static_cast<std::size_t>(index)];
        if (item.kind == ItemKind::None) continue;
        SDL_FRect icon{x + 26.0F, y + 5.0F, 22.0F, 22.0F};
        SDL_RenderTexture(renderer, texture_for(graphics, item_sprite(item.kind)), nullptr, &icon);
        text(renderer, x + 55.0F, y + 11.0F, item_name(item.kind));
    }
}

void inventory_overlay(SDL_Renderer* renderer, const GameGraphics& graphics,
                       const Game& game, int owner, const InteractionUi& ui) {
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr) return;
    shade(renderer);
    inventory_rows(renderer, graphics, *player, ui);
    const Item& focused = player->inventory.slots[static_cast<std::size_t>(ui.slot_focus)];
    const float shift = (1.0F - ui.slide) * 220.0F;
    const bool shop_offer = game.run.phase == RunPhase::Shop && ui.offer_focus < 3 &&
        game.run.shop_stock[static_cast<std::size_t>(ui.offer_focus)] != ItemKind::None;
    const bool comparing = has_reward_offer(game, owner) || shop_offer;
    if (comparing) {
        const Reward offer = shop_offer ? Reward{RewardKind::Item,
            game.run.shop_stock[static_cast<std::size_t>(ui.offer_focus)],
            ArtifactKind::None, 1} : reward_offer(game, owner, ui.offer_focus);
        reward_card(renderer, graphics, *player, offer,
                    204.0F + shift, 77.0F, 194.0F, false, "OFFER");
        draw_item_details(renderer, graphics, *player, focused,
                          413.0F + shift, 77.0F, 199.0F, 214.0F, "YOUR SLOT");
        if (offer.kind == RewardKind::Item && focused.kind != ItemKind::None) {
            const ItemPattern gain = item_pattern(offer.item);
            const ItemPattern held = item_pattern(focused.kind);
            char delta[96];
            std::snprintf(delta, sizeof(delta), "COMPARE  DMG %+d   REACH %+d   CD %+.2fs",
                          gain.damage - held.damage,
                          gain.maximum - held.maximum,
                          static_cast<double>(gain.cooldown - held.cooldown) / 60.0);
            text(renderer, 208.0F, 300.0F, delta, 136, 213, 147);
        }
    } else {
        draw_item_details(renderer, graphics, *player, focused,
                          204.0F + shift, 77.0F, 194.0F, 214.0F, "INSPECT");
        if (ui.slot_focus != player->inventory.selected)
            draw_item_details(renderer, graphics, *player, *player->inventory.held(),
                              413.0F + shift, 77.0F, 199.0F, 214.0F, "HELD");
    }
    text(renderer, 26.0F, 321.0F, "X / E / DEL DROP   A / ENTER EQUIP", 218, 198, 152);
    text(renderer, 425.0F, 321.0F, "Q / Y / B BACK", 218, 198, 152);
    if (!ui.notice.empty()) text(renderer, 26.0F, 342.0F, ui.notice, 231, 111, 87);
}

void offer_overlay(SDL_Renderer* renderer, const GameGraphics& graphics,
                   const Game& game, int owner, const InteractionUi& ui) {
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
    if (player == nullptr) return;
    shade(renderer);
    const bool shop = game.run.phase == RunPhase::Shop;
    const bool pending = game.run.phase == RunPhase::Playing;
    text(renderer, 25.0F, 26.0F, shop ? "TRAVELING SHOP" :
         (pending ? "CHOOSE MISSED REWARD" : "CHOOSE ONE REWARD"));
    if (shop) {
        char coins[40];
        std::snprintf(coins, sizeof(coins), "COINS %d",
                      game.run.coins[static_cast<std::size_t>(owner)]);
        text(renderer, 518.0F, 26.0F, coins, 224, 183, 112);
    }
    if (!shop && game.run.chosen[static_cast<std::size_t>(owner)] && !pending) {
        text(renderer, 210.0F, 166.0F, "WAITING FOR FRIENDS");
        return;
    }
    for (int index = 0; index < 3; ++index) {
        const float x = 22.0F + static_cast<float>(index) * 202.0F;
        const std::string label = std::to_string(index + 1) +
            (index == ui.offer_focus ? "  SELECTED" : "  CHOOSE");
        if (shop) {
            const ItemKind kind = game.run.shop_stock[static_cast<std::size_t>(index)];
            if (kind == ItemKind::None) {
                frame(renderer, x, 82.0F, 187.0F, 214.0F, index == ui.offer_focus);
                text(renderer, x + 62.0F, 185.0F, "SOLD");
            } else {
                reward_card(renderer, graphics, *player,
                            {RewardKind::Item, kind, ArtifactKind::None, 1},
                            x, 82.0F, 187.0F, index == ui.offer_focus, label.c_str());
                char price[40];
                std::snprintf(price, sizeof(price), "%d COINS", shop_price(kind));
                text(renderer, x + 12.0F, 286.0F, price, 224, 183, 112);
            }
        } else reward_card(renderer, graphics, *player, reward_offer(game, owner, index),
                           x, 82.0F, 187.0F, index == ui.offer_focus, label.c_str());
    }
    text(renderer, 25.0F, 321.0F, "1-3 / A CHOOSE    Q / Y COMPARE & MANAGE PACK",
         218, 198, 152);
    if (shop)
        text(renderer, 445.0F, 341.0F, ui.offer_focus == 3 ?
             "> CONTINUE" : "CONTINUE >", 218, 198, 152);
    else if (!ui.notice.empty())
        text(renderer, 25.0F, 342.0F, ui.notice, 231, 111, 87);
}

} // namespace

void draw_interaction(SDL_Renderer* renderer, const GameGraphics& graphics,
                      const Game& game, int owner, const InteractionUi& ui) {
    if (owner < 0 || owner >= 4) return;
    if (ui.inventory_open) inventory_overlay(renderer, graphics, game, owner, ui);
    else if (has_reward_offer(game, owner) || game.run.phase == RunPhase::Shop)
        offer_overlay(renderer, graphics, game, owner, ui);
}
