#include "flight_render.hpp"

#include <algorithm>
#include <cmath>

void apply_flight_pose(const Entity& bird, std::uint64_t tick, SDL_FRect& rect, double& angle) {
    if (bird.health <= 0 || bird.sleep_ticks > 0 || bird.stun_ticks > 0) return;
    const float beat = static_cast<float>(tick % 600);
    if (bird.kind == EntityKind::Mosquito || bird.kind == EntityKind::Wasp) {
        rect.x += rect.w*.15F; rect.y += rect.h*.15F;
        rect.w *= .7F; rect.h *= .7F;
        rect.y -= rect.h*.08F*std::sin(beat*.8F);
        const float wing = .86F+.14F*std::sin(beat*1.4F);
        rect.x += rect.w*(1-wing)*.5F; rect.w *= wing;
        if (bird.label_a == 1) angle += std::sin(static_cast<double>(beat))*6;
    } else if (bird.kind == EntityKind::FrostBat && bird.sprite == Sprite::FrostBatFlying) {
        const float wing = .8F + .2F*std::cos(beat*.65F);
        rect.x += rect.w*(1-wing)*.5F; rect.w *= wing;
    } else if (bird.kind == EntityKind::CarrionCrow) {
        const float wing = bird.move_wait > 0 ? .84F + .16F*std::cos(beat*.65F) : 1.0F;
        rect.x += rect.w*(1-wing)*.5F; rect.w *= wing;
        if (bird.label_a == 1) angle += std::sin(static_cast<double>(beat)*.8)*5;
    } else if (bird.kind == EntityKind::WaspNest && bird.counter_b <= 40 && bird.counter_a > 0) {
        angle += std::sin(static_cast<double>(beat)*.9)*3;
    } else if (bird.kind == EntityKind::Owl) {
        if (bird.label_a == 1) { rect.h *= .90F; rect.y += rect.h*.10F; }
        if (bird.label_a == 2) {
            const float wing = .82F+.18F*std::cos(beat*.7F);
            rect.x += rect.w*(1-wing)*.5F; rect.w *= wing;
            rect.y -= rect.h*.09F;
        }
    } else if (bird.kind == EntityKind::Woodpecker) {
        if (bird.label_a == 1 || bird.label_a == 2)
            rect.x += rect.w*.035F*std::sin(beat*1.8F)*(bird.facing.x < 0 ? -1 : 1);
        else if (bird.label_a == 3 && bird.timer_a > 55) angle += std::sin(static_cast<double>(beat)*.5)*9;
    }
}

// LANDING: A small wing shadow marks the remembered cell, not a full debug grid.
void draw_owl_landing(SDL_Renderer* renderer, const GameGraphics& graphics, const Game& game,
    ViewCamera camera, float zoom, const LightingCache& lighting) {
    SDL_Texture* texture = texture_for(graphics, Sprite::OwlFlying);
    for (const Entity& owl : game.entities) {
        if (owl.kind != EntityKind::Owl || owl.health <= 0 || owl.sleep_ticks > 0 ||
            owl.stun_ticks > 0 || (owl.label_a != 1 && owl.label_a != 2)) continue;
        SDL_FRect rect = tile_rect(owl.point_b, camera, zoom);
        if (rect.x < -rect.w || rect.x > 640 || rect.y < -rect.h || rect.y > 360) continue;
        const LightColor light = lit_sprite_color(lighting, owl.point_b);
        SDL_SetTextureColorModFloat(texture, light.red*.18F, light.green*.18F, light.blue*.18F);
        SDL_SetTextureAlphaMod(texture, 180);
        rect.x += rect.w*.16F; rect.y += rect.h*.3F; rect.w *= .68F; rect.h *= .4F;
        SDL_RenderTexture(renderer, texture, nullptr, &rect);
    }
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
    SDL_SetTextureAlphaMod(texture, 255);
}
