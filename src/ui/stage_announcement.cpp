#include "stage_announcement.hpp"
#include <algorithm>
#include <cstdio>

void update_stage_announcement(StageAnnouncement& banner, const Game& game,
                               float dt, bool playing, unsigned int revision) {
    if (!playing || !game.started) { banner.seed = 0; banner.floor = 0; banner.age = 10; return; }
    if (game.run.floor <= 0) return;
    if (banner.seed != game.run.seed || banner.floor != game.run.floor || banner.revision != revision) {
        banner.seed = game.run.seed; banner.floor = game.run.floor; banner.revision = revision;
        banner.age = 0;
        const int biome = (game.run.floor - 1) / 4;
        const char* name = biome == 0 ? "FOREST" : biome == 1 ? "FIRE" : "ICE";
        char label[48];
        std::snprintf(label, sizeof(label), "%s %d-%d", name, biome+1, (game.run.floor-1)%4+1);
        banner.title = label; banner.subtitle.clear();
        for (const Entity& entity : game.entities)
            if (entity.kind == EntityKind::Encounter) { banner.subtitle = "HAUNTED HOUSE"; break; }
    } else banner.age += std::clamp(dt, 0.0F, .1F);
}

// PRESENTATION: Compact angled strip below the zoom/FPS; no simulation timer or modal input.
void draw_stage_announcement(SDL_Renderer* renderer, const StageAnnouncement& banner) {
    if (banner.age >= 3.5F || banner.title.empty()) return;
    const float enter = std::clamp(banner.age / .25F, 0.0F, 1.0F);
    const float fade = enter * std::clamp((3.5F-banner.age) / .8F, 0.0F, 1.0F);
    const float y = 34 - 5*(1-enter)*(1-enter);
    const float height = banner.subtitle.empty() ? 20.0F : 32.0F;
    const auto quad = [&](float offset, SDL_FColor color) {
        SDL_Vertex vertices[]{{{450+offset,y+offset},color,{}},{{632+offset,y+offset},color,{}},
            {{627+offset,y+height+offset},color,{}},{{445+offset,y+height+offset},color,{}}};
        constexpr int indices[]{0,1,2,0,2,3};
        SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    quad(2, {0,0,0,.6F*fade});
    quad(0, {.16F,.065F,.06F,.94F*fade});
    SDL_SetRenderDrawColor(renderer, 235, 230, 214, static_cast<Uint8>(255*fade));
    SDL_RenderDebugText(renderer, 458, y+6, banner.title.c_str());
    if (!banner.subtitle.empty()) {
        SDL_SetRenderDrawColor(renderer, 198, 181, 145, static_cast<Uint8>(255*fade));
        SDL_RenderDebugText(renderer, 458, y+18, banner.subtitle.c_str());
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
