#include "artifacts.hpp"
#include "../artifacts/catalog.hpp"
#include "text.hpp"

// OWNED ARTIFACTS: Small named HUD badges; inventory adds their concrete effects.
void draw_owned_artifacts(SDL_Renderer* renderer, const GameGraphics& graphics,
                          const Entity& player, float x, float y, bool details) {
    if (player.artifacts == 0) {
        if (details) small_ui_text(renderer, x, y, "ARTIFACTS: NONE YET", 150, 161, 146);
        return;
    }
    small_ui_text(renderer, x, details ? y-10 : y, "ARTIFACTS", 181, 159, 109);
    if (!details) y += 12;
    int index = 0;
    for (ArtifactKind kind : artifact_kinds) {
        if (!has_artifact(player, kind)) continue;
        const float left = x + (details ? static_cast<float>(index % 2) * 209 : 0);
        const float top = y + static_cast<float>(details ? index / 2 : index) * (details ? 26 : 17);
        const SDL_FRect panel{left, top, details ? 199.0F : 130.0F, details ? 24.0F : 15.0F};
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 25, 31, 27, 235);
        SDL_RenderFillRect(renderer, &panel);
        const SDL_FRect stripe{left, top, 2, panel.h};
        SDL_SetRenderDrawColor(renderer, 181, 151, 81, 255);
        SDL_RenderFillRect(renderer, &stripe);
        const SDL_FRect icon{left+5, top+2, 12, 12};
        SDL_RenderTexture(renderer, texture_for(graphics, artifact_icon(kind)), nullptr, &icon);
        small_ui_text(renderer, left+21, top+3, artifact_name(kind), 227, 199, 137);
        if (details)
            small_ui_text(renderer, left+21, top+14, artifact_summary(kind), 178, 192, 174);
        ++index;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
