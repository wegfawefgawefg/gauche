#include "artifacts.hpp"
#include "../artifacts/catalog.hpp"
#include "text.hpp"
#include <algorithm>
#include <string>

// OWNED ARTIFACTS: Small named HUD badges; inventory adds their concrete effects.
void draw_owned_artifacts(tr::Renderer* renderer, const GameGraphics& graphics,
                          const Entity& player, float x, float y, bool details) {
    if (player.artifacts == 0 && std::all_of(player.powers.begin(),player.powers.end(),[](auto n){return n==0;})) {
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
        const SDL_FRect panel{left, top, details ? 199.0F : 172.0F, details ? 24.0F : 15.0F};
        tr::set_blend(renderer, SDL_BLENDMODE_BLEND);
        tr::set_color_bytes(renderer, 25, 31, 27, 235);
        tr::fill_rect(renderer, &panel);
        const SDL_FRect stripe{left, top, 2, panel.h};
        tr::set_color_bytes(renderer, 181, 151, 81, 255);
        tr::fill_rect(renderer, &stripe);
        const SDL_FRect icon{left+5, top+2, 12, 12};
        tr::draw_texture(renderer, texture_for(graphics, artifact_icon(kind)), nullptr, &icon);
        const std::string label=std::string(artifact_name(kind))+" x"+std::to_string(artifact_count(player,kind));
        small_ui_text(renderer, left+21, top+3, label, 227, 199, 137);
        if (details)
            small_ui_text(renderer, left+21, top+14, artifact_summary(kind), 178, 192, 174);
        ++index;
    }
    tr::set_blend(renderer, SDL_BLENDMODE_NONE);
}
