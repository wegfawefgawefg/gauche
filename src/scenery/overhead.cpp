#include "overhead.hpp"

#include <cmath>
#include <vector>

namespace {

struct Cutout {
    std::vector<SDL_FRect> rectangles;
    Cutout() {
        constexpr int bayer[4][4]{{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
        // VISIBILITY: A broad clear ellipse dissolves into sparse two-pixel holes.
        for (int y = 0; y < 360; y += 2) {
            int start = -1;
            for (int x = 0; x <= 640; x += 2) {
                const float dx = (static_cast<float>(x) - 320) / 205;
                const float dy = (static_cast<float>(y) - 180) / 122;
                const float radial = std::sqrt(dx * dx + dy * dy);
                const bool clear = x < 640 && (radial < 1 ||
                    (radial < 1.32F && (1.32F - radial) / .32F >
                        (static_cast<float>(bayer[(y / 2) % 4][(x / 2) % 4]) + .5F) / 16));
                if (clear && start < 0) start = x;
                if (!clear && start >= 0) {
                    rectangles.push_back({static_cast<float>(start), static_cast<float>(y),
                        static_cast<float>(x - start), 2});
                    start = -1;
                }
            }
        }
    }
};

std::uint32_t tree_bits(int x, int y, std::uint64_t seed) {
    std::uint64_t value = static_cast<std::uint64_t>(static_cast<std::int64_t>(x)) * std::uint64_t{747796405} ^
        static_cast<std::uint64_t>(static_cast<std::int64_t>(y)) * std::uint64_t{2891336453} ^ seed;
    value ^= value >> 30; value *= 0xbf58476d1ce4e5b9ULL; value ^= value >> 27;
    return static_cast<std::uint32_t>(value);
}

void forest_canopies(SDL_Renderer* renderer, const GameGraphics& graphics,
                      const Game& game, ViewCamera camera, float zoom, const LightingCache& lighting) {
    const float pixels = tile_pixels(zoom);
    const int grid_x = static_cast<int>(std::floor(camera.x / 6));
    const int grid_y = static_cast<int>(std::floor(camera.y / 6));
    const int columns = static_cast<int>(std::ceil(320 / (pixels * 6))) + 2;
    const int rows = static_cast<int>(std::ceil(180 / (pixels * 6))) + 2;
    for (int gy = grid_y - rows; gy <= grid_y + rows; ++gy)
        for (int gx = grid_x - columns; gx <= grid_x + columns; ++gx) {
            const std::uint32_t bits = tree_bits(gx, gy, game.run.seed);
            const Cell anchor{gx * 6 + static_cast<int>(bits % 3), gy * 6 + static_cast<int>((bits >> 4) % 3)};
            if (!game.stage.in_bounds(anchor) || game.stage.at_or_border(anchor).kind != TileKind::Wall || bits % 5 == 0) continue;
            const float depth = 1.08F + static_cast<float>((bits >> 8) % 5) * .015F;
            const float size = pixels * (7 + static_cast<float>((bits >> 16) % 4));
            const float x = view_center_x + (static_cast<float>(anchor.x) - camera.x) * pixels * depth;
            const float y = view_center_y + (static_cast<float>(anchor.y) - camera.y) * pixels * depth;
            const SDL_FRect rect{x - size * .5F, y - size * .5F, size, size};
            if (rect.x + size < 0 || rect.y + size < 0 || rect.x > 640 || rect.y > 360) continue;
            SDL_Texture* texture = texture_for(graphics, bits % 2 == 0 ? Sprite::CanopyOak : Sprite::CanopyPine);
            const LightColor light = light_at_cell(lighting, anchor);
            SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
            SDL_SetTextureAlphaMod(texture, 148);
            SDL_RenderTextureRotated(renderer, texture, nullptr, &rect,
                static_cast<double>((bits >> 22) % 4) * 90, nullptr, SDL_FLIP_NONE);
            SDL_SetTextureAlphaMod(texture, 255);
            SDL_SetTextureColorModFloat(texture, 1, 1, 1);
        }
}

} // namespace

void draw_overhead(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, const Cosmetics* cosmetics, ViewCamera camera,
                    float zoom, const LightingCache& lighting) {
    if (graphics.overhead_canvas == nullptr) {
        graphics.overhead_canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, 640, 360);
        if (graphics.overhead_canvas == nullptr) return;
        SDL_SetTextureBlendMode(graphics.overhead_canvas, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(graphics.overhead_canvas, SDL_SCALEMODE_NEAREST);
    }
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    float sx = 1, sy = 1;
    SDL_GetRenderScale(renderer, &sx, &sy);
    SDL_SetRenderTarget(renderer, graphics.overhead_canvas);
    SDL_SetRenderScale(renderer, 1, 1);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    if (game.run.phase != RunPhase::Arena && game.run.floor <= 4)
        forest_canopies(renderer, graphics, game, camera, zoom, lighting);
    else if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Weather, camera, zoom);
    // MASK: Clear alpha with unblended fills; transparent geometry can be discarded by software renderers.
    static const Cutout cutout;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRects(renderer, cutout.rectangles.data(), static_cast<int>(cutout.rectangles.size()));
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderScale(renderer, sx, sy);
    const SDL_FRect rect{0, 0, 640, 360};
    SDL_RenderTexture(renderer, graphics.overhead_canvas, nullptr, &rect);
}
