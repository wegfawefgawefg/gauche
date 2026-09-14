#include "system.hpp"
#include "../surfaces/interaction.hpp"
#include "../view.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float pi = 3.14159265358979323846F;

float screen_x(float x, ViewCamera camera, float pixels, float parallax) {
    return view_center_x + (x - camera.x) * parallax * pixels;
}

float screen_y(float y, ViewCamera camera, float pixels, float parallax) {
    return view_center_y + (y - camera.y) * parallax * pixels;
}

void draw_sprite(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const SpriteParticle& particle, ViewCamera camera, float pixels,
                 const LightingCache* lighting) {
    const float age = static_cast<float>(particle.span - particle.life);
    const float progress = particle.span > 0 ? age / static_cast<float>(particle.span) : 0.0F;
    const float curve = particle.motion == ParticleMotion::Arc ?
        std::sin(progress * pi) * particle.arc : 0.0F;
    const float parallax = 1.0F + static_cast<float>(particle.depth_height) * 0.005F;
    const float x = screen_x(particle.x, camera, pixels, parallax);
    const float y = screen_y(particle.y - curve, camera, pixels, parallax);
    const SDL_FRect rect{x - particle.width * pixels * 0.5F,
                         y - particle.height * pixels * 0.5F,
                         particle.width * pixels, particle.height * pixels};
    if (rect.x > 640.0F || rect.y > 360.0F ||
        rect.x + rect.w < 0.0F || rect.y + rect.h < 0.0F) return;
    const Sprite id = particle.motion == ParticleMotion::Animated &&
                      (static_cast<int>(age) / 12) % 2 != 0 ?
                      particle.next_sprite : particle.sprite;
    SDL_Texture* texture = texture_for(graphics, id);
    if (lighting != nullptr && particle.layer != ParticleLayer::Weather) {
        const LightColor light = light_at_cell(*lighting,
            {static_cast<int>(std::floor(particle.x)),
             static_cast<int>(std::floor(particle.y))});
        SDL_SetTextureColorModFloat(texture, std::max(light.red, particle.self_glow.red),
            std::max(light.green, particle.self_glow.green),
            std::max(light.blue, particle.self_glow.blue));
    }
    const float fade = particle.layer == ParticleLayer::Weather ?
        static_cast<float>(particle.life) / static_cast<float>(particle.span) :
        std::min(1.0F, static_cast<float>(particle.life) /
                       std::max(1.0F, static_cast<float>(particle.span) * 0.25F));
    SDL_SetTextureAlphaMod(texture, static_cast<std::uint8_t>(
        std::clamp(particle.alpha * fade, 0.0F, 1.0F) * 255.0F));
    SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, particle.angle,
                             nullptr, SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(texture, 255);
    SDL_SetTextureColorModFloat(texture, 1.0F, 1.0F, 1.0F);
}

void draw_ribbon(SDL_Renderer* renderer, const RibbonParticle& ribbon,
                 ViewCamera camera, float pixels) {
    if (ribbon.count < 2) return;
    const auto alpha = static_cast<std::uint8_t>(
        220 * ribbon.life / std::max(1, ribbon.span));
    SDL_SetRenderDrawColor(renderer, ribbon.red, ribbon.green, ribbon.blue, alpha);
    for (int index = 1; index < ribbon.count; ++index) {
        const SDL_FPoint first = ribbon.points[static_cast<std::size_t>(index - 1)];
        const SDL_FPoint second = ribbon.points[static_cast<std::size_t>(index)];
        SDL_RenderLine(renderer,
            screen_x(first.x, camera, pixels, 1.0F),
            screen_y(first.y, camera, pixels, 1.0F),
            screen_x(second.x, camera, pixels, 1.0F),
            screen_y(second.y, camera, pixels, 1.0F));
    }
}

void draw_ring(SDL_Renderer* renderer, const RingParticle& ring,
               ViewCamera camera, float pixels, const LightingCache* lighting, const Stage* stage) {
    const auto alpha = static_cast<std::uint8_t>(
        205 * ring.life / std::max(1, ring.span));
    const LightColor light = lighting != nullptr && ring.water ? light_at_cell(*lighting,
        {static_cast<int>(ring.x), static_cast<int>(ring.y)}) : LightColor{1, 1, 1};
    SDL_SetRenderDrawColorFloat(renderer, light.red * static_cast<float>(ring.red) / 255,
        light.green * static_cast<float>(ring.green) / 255,
        light.blue * static_cast<float>(ring.blue) / 255, static_cast<float>(alpha) / 255);
    const float cx = screen_x(ring.x, camera, pixels, 1.0F);
    const float cy = screen_y(ring.y, camera, pixels, 1.0F);
    for (int index = 0; index < 24; ++index) {
        const float first = 2.0F * pi * static_cast<float>(index) / 24.0F;
        const float second = 2.0F * pi * static_cast<float>(index + 1) / 24.0F;
        // BANK: Ripples end at the shoreline instead of crossing walls and dry paths.
        const float height = ring.water ? .65F : 1;
        if (ring.water && stage != nullptr) {
            const Cell cell{static_cast<int>(std::floor(ring.x + std::cos(first) * ring.radius)),
                static_cast<int>(std::floor(ring.y + std::sin(first) * ring.radius * height))};
            if (!surface_wet(stage->at_or_border(cell))) continue;
        }
        SDL_RenderLine(renderer,
            cx + std::cos(first) * ring.radius * pixels,
            cy + std::sin(first) * ring.radius * pixels * height,
            cx + std::cos(second) * ring.radius * pixels,
            cy + std::sin(second) * ring.radius * pixels * height);
    }
}

} // namespace

void draw_particles(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Cosmetics& cosmetics, ParticleLayer layer, ViewCamera camera,
                    float zoom, const LightingCache* lighting, const Stage* stage) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const float pixels = tile_pixels(zoom);
    for (const SpriteParticle& particle : cosmetics.sprites)
        if (particle.layer == layer)
            draw_sprite(renderer, graphics, particle, camera, pixels, lighting);
    for (const RibbonParticle& ribbon : cosmetics.ribbons)
        if (ribbon.layer == layer) draw_ribbon(renderer, ribbon, camera, pixels);
    for (const RingParticle& ring : cosmetics.rings)
        if (ring.layer == layer) draw_ring(renderer, ring, camera, pixels, lighting, stage);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
