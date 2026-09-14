#include "render.hpp"
#include "../lighting/render.hpp"

#include <algorithm>
#include <cmath>

void draw_projectile(SDL_Renderer* renderer, const GameGraphics& graphics,
                     const Entity& shot, const Game& game, ViewCamera camera,
                     float zoom, const LightingCache& lighting) {
    const bool hook = shot.label_a == static_cast<int>(ProjectileKind::Hook);
    const bool bomb = shot.label_a == static_cast<int>(ProjectileKind::Bomb);
    const bool rocket = shot.label_a == static_cast<int>(ProjectileKind::Rocket);
    const bool thrown = bomb || shot.label_a == static_cast<int>(ProjectileKind::Flask);
    const float travel = shot.counter_a > 0 && (!hook || shot.label_b == 0) ?
        1 - static_cast<float>(shot.timer_b) / static_cast<float>(projectile_step_ticks(shot)) : 0;
    const float pixels = tile_pixels(zoom);
    SDL_FRect rect = tile_rect(shot.cell, camera, zoom);
    rect.x += static_cast<float>(shot.facing.x) * travel * pixels;
    rect.y += static_cast<float>(shot.facing.y) * travel * pixels;
    if (thrown && shot.counter_a > 0) {
        const float progress = (static_cast<float>(shot.attack_interval - shot.counter_a) + travel) /
            static_cast<float>(std::max(1, shot.attack_interval));
        rect.y -= pixels * .85F * std::sin(progress * 3.14159265F);
    }
    rect.x += pixels * .17F; rect.y += pixels * .17F;
    rect.w = rect.h = pixels * .66F;
    SDL_Texture* texture = texture_for(graphics, shot.sprite);
    const LightColor light = lit_sprite_color(lighting, shot.cell);
    SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
    const double angle = thrown ? (shot.counter_a > 0 ? static_cast<double>(game.tick % 60) * 9 : 0) :
        shot.facing.x > 0 ? 0 : shot.facing.x < 0 ? 180 : shot.facing.y > 0 ? 90 : -90;
    if (hook) {
        if (const Entity* owner = get_entity(game, shot.entity_a)) {
            const SDL_FRect hand = tile_rect(owner->cell, camera, zoom);
            SDL_SetRenderDrawColorFloat(renderer, light.red * .57F, light.green * .47F, light.blue * .31F, 1);
            SDL_RenderLine(renderer, hand.x + hand.w * .5F, hand.y + hand.h * .5F,
                rect.x + rect.w * .5F, rect.y + rect.h * .5F);
        }
    }
    SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
    if (rocket) {
        SDL_SetRenderDrawColor(renderer, 255, 181, 76, 255);
        const float x = rect.x + rect.w * .5F, y = rect.y + rect.h * .5F;
        SDL_RenderLine(renderer, x - static_cast<float>(shot.facing.x) * pixels * .35F,
            y - static_cast<float>(shot.facing.y) * pixels * .35F,
            x - static_cast<float>(shot.facing.x) * pixels * .65F,
            y - static_cast<float>(shot.facing.y) * pixels * .65F);
    }
    if (bomb) {
        // FUSE: A few local sparks communicate danger without a debug attack grid.
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < 3; ++i) {
            const float beat = static_cast<float>((game.tick + static_cast<std::uint64_t>(i * 5)) % 17) / 17;
            SDL_SetRenderDrawColor(renderer, 255, static_cast<Uint8>(180 - i * 25), 62,
                static_cast<Uint8>(230 * (1-beat)));
            SDL_RenderPoint(renderer, rect.x + rect.w * .68F + pixels * beat * (i == 1 ? -.18F : .12F),
                rect.y + rect.h * .1F - pixels * beat * .35F);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}
