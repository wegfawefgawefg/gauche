#include "plant_render.hpp"
#include "attacks.hpp"

#include <algorithm>

void draw_plant_lash(tr::Renderer* renderer, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting) {
    tr::set_blend(renderer, SDL_BLENDMODE_BLEND);
    for (const Entity& root : game.entities) {
        if (root.kind != EntityKind::RootTurret || root.health <= 0 ||
            root.label_a != 2 || root.timer_a < 80) continue;
        const SDL_FRect start = tile_rect(root.cell, camera, zoom);
        SDL_FPoint previous{start.x+start.w*.5F, start.y+start.h*.5F};
        const float fade = static_cast<float>(root.timer_a-79)/11;
        for (int step = 1; step <= root.counter_a*2; ++step) {
            const int reach = (step+1)/2;
            const Cell cell = root.cell+Cell{root.facing.x*reach, root.facing.y*reach};
            if (!clear_sight(game, root.cell, cell, false)) break;
            const LightColor light = lit_sprite_color(lighting, cell);
            const auto channel = [](float value) { return static_cast<Uint8>(std::clamp(value, 0.0F, 255.0F)); };
            tr::set_color_bytes(renderer, channel(light.red*170), channel(light.green*183),
                channel(light.blue*86), channel(fade*255));
            const float along = static_cast<float>(step)*.5F;
            const float side = (step%2 == 0 ? .07F : -.07F)*fade;
            const SDL_FPoint next{start.x+start.w*(.5F+static_cast<float>(root.facing.x)*along-static_cast<float>(root.facing.y)*side),
                start.y+start.h*(.5F+static_cast<float>(root.facing.y)*along+static_cast<float>(root.facing.x)*side)};
            tr::line(renderer, previous.x, previous.y, next.x, next.y);
            previous = next;
        }
    }
    tr::set_blend(renderer, SDL_BLENDMODE_NONE);
}

// AIM: A tiny bright bud turns with the root; the full lane remains a debug overlay.
void draw_root_head(tr::Renderer* renderer, const Entity& root, SDL_FRect rect, LightColor light) {
    const float size = root.label_a == 1 ? .18F : .12F;
    SDL_FRect bud{rect.x+rect.w*(.5F + static_cast<float>(root.facing.x)*.19F-size*.5F),
        rect.y+rect.h*(.45F + static_cast<float>(root.facing.y)*.19F-size*.5F), rect.w*size, rect.h*size};
    const auto channel = [](float value) { return static_cast<Uint8>(std::clamp(value, 0.0F, 255.0F)); };
    tr::set_color_bytes(renderer, channel(light.red*225), channel(light.green*199), channel(light.blue*120), 255);
    tr::fill_rect(renderer, &bud);
}
