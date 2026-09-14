#include "status.hpp"
#include "text.hpp"

#include <algorithm>
#include <array>
#include <cstdio>

namespace {

struct StatusRow {
    const char* name;
    int ticks;
    SDL_Color color;
    const char* effect;
    Sprite icon;
};

} // namespace

void draw_player_status(SDL_Renderer* renderer, const GameGraphics& graphics,
                         const Game& game, const Entity& player, float x, float bottom) {
    if (player.health <= 0) return;
    char burning[80];
    const int weak = static_cast<int>((game.tick + static_cast<std::uint64_t>(player.scorch_ticks)) / 30 - game.tick / 30) * 2;
    const int strong = static_cast<int>((game.tick + static_cast<std::uint64_t>(player.burn_ticks)) / 30 - game.tick / 30) * 4;
    const int per_second = (player.scorch_ticks > 0 ? 4 : 0) + (player.burn_ticks > 0 ? 8 : 0);
    std::snprintf(burning, sizeof(burning), "%d HP/S NOW | %d DMG LEFT", per_second, weak + strong);
    const std::array rows{
        StatusRow{"BURNING", std::max(player.scorch_ticks, player.burn_ticks),
                  {233, 150, 76, 255}, burning, Sprite::FlameA},
        StatusRow{"ASLEEP", player.sleep_ticks, {184, 164, 224, 255},
                  "NO ACTIONS | DAMAGE WAKES", Sprite::StatusSleep},
        StatusRow{"STUNNED", player.stun_ticks, {231, 198, 91, 255},
                  "NO MOVEMENT OR ACTIONS", Sprite::StatusStun},
        StatusRow{"CHILLED", player.freeze_ticks, {121, 191, 230, 255},
                  "STEP RECOVERY HALF SPEED", Sprite::StatusChill},
    };
    for (const StatusRow& row : rows) {
        if (row.ticks <= 0) continue;
        bottom -= 25;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_FRect panel{x, bottom, 201, 23};
        SDL_SetRenderDrawColor(renderer, 16, 21, 22, 225);
        SDL_RenderFillRect(renderer, &panel);
        SDL_FRect stripe{x, bottom, 2, 23};
        SDL_SetRenderDrawColor(renderer, row.color.r, row.color.g, row.color.b, 235);
        SDL_RenderFillRect(renderer, &stripe);
        SDL_FRect icon{x + 4, bottom + 4, 13, 13};
        SDL_RenderTexture(renderer, texture_for(graphics, row.icon), nullptr, &icon);
        char label[64];
        std::snprintf(label, sizeof(label), "%s  %.1fs", row.name, static_cast<double>(row.ticks) / 60.0);
        small_ui_text(renderer, x + 20, bottom + 3, label, row.color.r, row.color.g, row.color.b);
        small_ui_text(renderer, x + 20, bottom + 13, row.effect, 187, 190, 179);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}
