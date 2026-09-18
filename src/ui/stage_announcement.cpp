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
        const Biome biome=floor_biome(game.run.floor);
        const char* name=biome_name(biome);
        char label[48];
        std::snprintf(label, sizeof(label), "%s %d-%d", name, static_cast<int>(biome)+1, biome_stage(game.run.floor));
        banner.title = label; banner.subtitle.clear();
        if (game.run.layout==FloorLayout::HauntedHouse) banner.subtitle="HAUNTED HOUSE";
        if (game.run.layout==FloorLayout::FreightExchange) banner.subtitle="FREIGHT EXCHANGE";
        if (game.run.layout==FloorLayout::LastShift) banner.subtitle="THE LAST SHIFT";
    } else banner.age += std::clamp(dt, 0.0F, .1F);
}

// PRESENTATION: Compact angled strip below the zoom/FPS; no simulation timer or modal input.
void draw_stage_announcement(tr::Renderer* renderer, const StageAnnouncement& banner) {
    if (banner.age >= 3.5F || banner.title.empty()) return;
    const float enter = std::clamp(banner.age / .25F, 0.0F, 1.0F);
    const float fade = enter * std::clamp((3.5F-banner.age) / .8F, 0.0F, 1.0F);
    const float y = 34 - 5*(1-enter)*(1-enter);
    const float height = banner.subtitle.empty() ? 20.0F : 32.0F;
    const auto quad = [&](float offset, SDL_FColor color) {
        SDL_Vertex vertices[]{{{450+offset,y+offset},color,{}},{{632+offset,y+offset},color,{}},
            {{627+offset,y+height+offset},color,{}},{{445+offset,y+height+offset},color,{}}};
        constexpr int indices[]{0,1,2,0,2,3};
        tr::geometry(renderer, nullptr, vertices, 4, indices, 6);
    };
    tr::set_blend(renderer, SDL_BLENDMODE_BLEND);
    quad(2, {0,0,0,.6F*fade});
    quad(0, {.16F,.065F,.06F,.94F*fade});
    tr::set_color_bytes(renderer, 235, 230, 214, static_cast<Uint8>(255*fade));
    tr::debug_text(renderer, 458, y+6, banner.title.c_str());
    if (!banner.subtitle.empty()) {
        tr::set_color_bytes(renderer, 198, 181, 145, static_cast<Uint8>(255*fade));
        tr::debug_text(renderer, 458, y+18, banner.subtitle.c_str());
    }
    tr::set_blend(renderer, SDL_BLENDMODE_NONE);
}
