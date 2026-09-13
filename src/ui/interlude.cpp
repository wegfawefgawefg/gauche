#include "presentation.hpp"

#include <cstdio>

namespace {
void sprite(SDL_Renderer* renderer, const GameGraphics& graphics, Sprite id, SDL_FRect rect) {
    SDL_RenderTexture(renderer, texture_for(graphics, id), nullptr, &rect);
}
const char* artifact_name(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::AllPiercing: return "All Piercing";
    case ArtifactKind::Reflector: return "Reflector";
    case ArtifactKind::Hearth: return "Hearth";
    case ArtifactKind::FleetFeet: return "Fleet Feet";
    default: return "Artifact";
    }
}

} // namespace

void draw_interlude(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, int local_owner) {
    const bool pending = game.run.phase == RunPhase::Playing &&
                         game.run.pending_count[static_cast<std::size_t>(local_owner)] > 0;
    if (game.run.phase != RunPhase::Reward && game.run.phase != RunPhase::Shop &&
        game.run.phase != RunPhase::Won && !pending) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 8, 12, 13, 228);
    const SDL_FRect overlay{0.0F, 0.0F, 640.0F, 360.0F};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 235, 230, 214, 255);
    if (game.run.phase == RunPhase::Won) {
        SDL_RenderDebugText(renderer, 256.0F, 140.0F, "RUN CLEARED");
        return;
    }
    SDL_RenderDebugText(renderer, 35.0F, 32.0F,
                        pending ? "CHOOSE MISSED REWARD" :
                        (game.run.phase == RunPhase::Reward ?
                            "CHOOSE ONE REWARD" : "TRAVELING SHOP"));
    for (int index = 0; index < 3; ++index) {
        const float x = 35.0F + static_cast<float>(index) * 196.0F;
        SDL_FRect card{x, 70.0F, 178.0F, 165.0F};
        SDL_SetRenderDrawColor(renderer, 177, 159, 106, 255);
        SDL_RenderRect(renderer, &card);
        char key[8];
        std::snprintf(key, sizeof(key), "%d", index + 1);
        SDL_RenderDebugText(renderer, x + 10.0F, 81.0F, key);
        if (game.run.phase == RunPhase::Reward || pending) {
            const Reward reward = pending ?
                game.run.pending_offers[static_cast<std::size_t>(local_owner)][0]
                                       [static_cast<std::size_t>(index)] :
                game.run.offers[static_cast<std::size_t>(local_owner)]
                               [static_cast<std::size_t>(index)];
            const char* name = "";
            switch (reward.kind) {
            case RewardKind::Item:
                name = item_name(reward.item);
                sprite(renderer, graphics, item_sprite(reward.item),
                       {x + 69.0F, 111.0F, 40.0F, 40.0F});
                break;
            case RewardKind::Artifact: name = artifact_name(reward.artifact); break;
            case RewardKind::Health: name = "+20 Max HP"; break;
            case RewardKind::Speed: name = "Faster Steps"; break;
            }
            SDL_RenderDebugText(renderer, x + 10.0F, 165.0F, name);
        } else {
            const ItemKind kind = game.run.shop_stock[static_cast<std::size_t>(index)];
            if (kind != ItemKind::None)
                sprite(renderer, graphics, item_sprite(kind),
                       {x + 69.0F, 111.0F, 40.0F, 40.0F});
            SDL_RenderDebugText(renderer, x + 10.0F, 165.0F, item_name(kind));
            if (kind != ItemKind::None) {
                char cost[24];
                std::snprintf(cost, sizeof(cost), "%d coins", shop_price(kind));
                SDL_RenderDebugText(renderer, x + 10.0F, 189.0F, cost);
            }
        }
    }
    if (game.run.phase == RunPhase::Reward || pending) {
        SDL_RenderDebugText(renderer, 35.0F, 265.0F, "1-3 CHOOSE    Q DROP HELD ITEM IF PACK FULL");
    } else {
        char coins[32];
        std::snprintf(coins, sizeof(coins), "COINS %d",
                      game.run.coins[static_cast<std::size_t>(local_owner)]);
        SDL_RenderDebugText(renderer, 35.0F, 265.0F, coins);
        SDL_RenderDebugText(renderer, 35.0F, 289.0F, "ENTER TO CONTINUE");
    }
}
