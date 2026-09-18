#include "../debug/performance.hpp"
#include "overhead.hpp"
#include "../debug/panels.hpp"

#include <cmath>
#include <algorithm>
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

// Cached mask; the secondary texture multiplies premultiplied canopy RGBA.
// Lighting textures have alpha=1, so they use the same shader unchanged.
tr::Texture* canopy_mask(tr::Renderer* renderer, const GameGraphics& graphics) {
    const auto& options = debug_panels();
    if (graphics.overhead_mask && graphics.overhead_opening == options.canopy_opening &&
        graphics.overhead_fade == options.canopy_fade) return graphics.overhead_mask;
    constexpr int width=160, height=90;
    if (!graphics.overhead_mask)
        graphics.overhead_mask = tr::create_texture(renderer, SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STATIC, width, height);
    if (!graphics.overhead_mask) return nullptr;
    std::vector<std::uint8_t> pixels(width*height*4);
    for (int y=0; y<height; ++y) for (int x=0; x<width; ++x) {
        const float dx=std::abs((static_cast<float>(x)+.5F)*2/width-1);
        const float dy=std::abs((static_cast<float>(y)+.5F)*2/height-1);
        const float distance=std::pow(std::pow(dx,6.F)+std::pow(dy,6.F),1.F/6);
        const float t=std::clamp((distance-options.canopy_opening)/options.canopy_fade,0.F,1.F);
        const auto fade=static_cast<std::uint8_t>(255*t*t*(3-2*t)+.5F);
        for (int channel=0; channel<4; ++channel)
            pixels[static_cast<std::size_t>((y*width+x)*4+channel)]=fade;
    }
    if (!tr::update_texture(graphics.overhead_mask,nullptr,pixels.data(),width*4)) return nullptr;
    graphics.overhead_opening=options.canopy_opening;
    graphics.overhead_fade=options.canopy_fade;
    return graphics.overhead_mask;
}

std::uint32_t tree_bits(int x, int y, std::uint64_t seed) {
    std::uint64_t value = static_cast<std::uint64_t>(static_cast<std::int64_t>(x)) * std::uint64_t{747796405} ^
        static_cast<std::uint64_t>(static_cast<std::int64_t>(y)) * std::uint64_t{2891336453} ^ seed;
    value ^= value >> 30; value *= 0xbf58476d1ce4e5b9ULL; value ^= value >> 27;
    return static_cast<std::uint32_t>(value);
}

float smooth_fraction(float value) {
    const float t=std::clamp(value,0.F,1.F);
    return t*t*(3-2*t);
}

// Evaluate the unshifted object reference, never individual sprite pixels.
// Positions derive from the camera, so slowing/stopping/reversing needs no timers.
float canopy_object(SDL_FRect& rect, std::uint32_t bits) {
    const auto& options=debug_panels();
    if (options.canopy_style < 4) return 1;
    const float x=rect.x+rect.w*.5F;
    const float y=rect.y+rect.h*(options.canopy_reference == 1 ? 1.F : .5F);
    float dx=x-view_center_x, dy=y-view_center_y;
    const float distance=std::hypot(dx/view_center_x,dy/view_center_y);
    // Slight per-tree variation keeps neighboring crowns from acting as a ring.
    const float variation=.9F+static_cast<float>((bits>>12)%101)*.002F;
    const float visible=smooth_fraction((distance-options.canopy_near*variation)/options.canopy_transition);
    if (options.canopy_style == 4) return visible;

    // Move the entire bounding box out along its approach direction. At the
    // center, either direction is safe: the tree is already completely offscreen.
    if (std::abs(dx)+std::abs(dy)<.001F) { dx=(bits&1) ? 1.F : -1.F; dy=0; }
    const float length=std::hypot(dx,dy);
    dx/=length; dy/=length;
    const float exit_x=dx>.0001F ? (640-rect.x)/dx :
        dx<-.0001F ? -(rect.x+rect.w)/dx : 1e6F;
    const float exit_y=dy>.0001F ? (360-rect.y)/dy :
        dy<-.0001F ? -(rect.y+rect.h)/dy : 1e6F;
    const float shift=std::max(0.F,std::min(exit_x,exit_y)+1)*(1-visible);
    rect.x+=dx*shift; rect.y+=dy*shift;
    return 1;
}

void forest_canopies(tr::Renderer* renderer, const GameGraphics& graphics,
                      const Game& game, ViewCamera camera, float zoom, const LightingCache& lighting) {
    const bool upright = debug_panels().canopy_upright;
    const float pixels = tile_pixels(zoom);
    const int grid_x = static_cast<int>(std::floor(camera.x / 6));
    const int grid_y = static_cast<int>(std::floor(camera.y / 6));
    const int columns = static_cast<int>(std::ceil(320 / (pixels * 6))) + 2;
    const int rows = static_cast<int>(std::ceil(180 / (pixels * 6))) + 3;
    for (int gy = grid_y - rows; gy <= grid_y + rows; ++gy)
        for (int gx = grid_x - columns; gx <= grid_x + columns; ++gx) {
            const std::uint32_t bits = tree_bits(gx, gy, game.run.seed);
            const Cell anchor{gx * 6 + static_cast<int>(bits % 3), gy * 6 + static_cast<int>((bits >> 4) % 3)};
            // Decorative parallax field continues over rooms and beyond map bounds.
            // Only the viewport and opening mask determine what is visible.
            if (bits % 5 == 0) continue;
            const float depth = 1.08F + static_cast<float>((bits >> 8) % 5) * .015F;
            const float size = pixels * (7 + static_cast<float>((bits >> 16) % 4));
            const float x = view_center_x + (static_cast<float>(anchor.x) - camera.x) * pixels * depth;
            const float y = view_center_y + (static_cast<float>(anchor.y) - camera.y) * pixels * depth;
            SDL_FRect rect{x - size * .5F, y - size * (upright ? 1.1F : .5F), size,
                size * (upright ? 4.F/3 : 1.F)};
            const float alpha=canopy_object(rect,bits);
            if (alpha <= 0) continue;
            if (rect.x + size < 0 || rect.y + rect.h < 0 || rect.x > 640 || rect.y > 360) continue;
            tr::Texture* texture = texture_for(graphics, bits % 2 == 0 ? Sprite::CanopyOak : Sprite::CanopyPine);
            const LightColor light = light_at_cell(lighting, anchor);
            // CANOPY: These leaves sit above the dark floor and receive skylight.
            // Keep their daylight color at one fifth intensity so the crown stays peripheral.
            tr::texture_color(texture, .20F*std::max(.80F, light.red),
                .20F*std::max(.88F, light.green), .20F*std::max(.72F, light.blue));
            tr::texture_alpha(texture, (225.F/255)*alpha);
            tr::draw_rotated(renderer, texture, nullptr, &rect,
                upright ? 0 : static_cast<double>((bits >> 22) % 4) * 90, nullptr,
                upright && (bits & 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
            tr::texture_alpha_bytes(texture, 255);
            tr::texture_color(texture, 1, 1, 1);
        }
}

} // namespace

void draw_overhead(tr::Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, const Cosmetics* cosmetics, ViewCamera camera,
                    float zoom, const LightingCache& lighting) {
    PerfScope perf_scope(PerfZone::Overhead);
    const bool forest=game.run.phase != RunPhase::Arena && forest_floor(game.run.floor);
    const int selected=debug_panels().canopy_style;
    const int style=!forest && selected>=4 ? 1 : selected;
    if (style == 3) return;
    if (graphics.overhead_canvas == nullptr) {
        graphics.overhead_canvas = tr::create_texture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, 640, 360);
        if (graphics.overhead_canvas == nullptr) return;
        tr::texture_blend(graphics.overhead_canvas, SDL_BLENDMODE_BLEND);
        tr::texture_filter(graphics.overhead_canvas, SDL_SCALEMODE_NEAREST);
    }
    tr::Texture* target = tr::get_target(renderer);
    float sx = 1, sy = 1;
    tr::get_scale(renderer, &sx, &sy);
    tr::set_target(renderer, graphics.overhead_canvas);
    tr::set_scale(renderer, 1, 1);
    tr::set_color_bytes(renderer, 0, 0, 0, 0);
    tr::clear(renderer);
    if (forest)
        forest_canopies(renderer, graphics, game, camera, zoom, lighting);
    else if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Weather, camera, zoom);
    if (style == 0) {
        static const Cutout cutout;
        tr::set_blend(renderer, SDL_BLENDMODE_NONE);
        tr::set_color_bytes(renderer, 0, 0, 0, 0);
        tr::fill_rects(renderer, cutout.rectangles.data(), static_cast<int>(cutout.rectangles.size()));
    }
    tr::set_target(renderer, target);
    tr::set_scale(renderer, sx, sy);
    const SDL_FRect rect{0, 0, 640, 360};
    // Render-target colors are already premultiplied; fade both color and alpha.
    tr::texture_blend(graphics.overhead_canvas, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    if (style == 1) {
        auto* mask=canopy_mask(renderer,graphics);
        if (!mask) return;
        const SDL_Vertex vertices[]{
            {{0,0},{1,1,1,1},{0,0}}, {{640,0},{1,1,1,1},{1,0}},
            {{640,360},{1,1,1,1},{1,1}}, {{0,360},{1,1,1,1},{0,1}}};
        const SDL_FPoint mask_uv[]{{0,0},{1,0},{1,1},{0,1}};
        constexpr int indices[]{0,1,2,0,2,3};
        tr::geometry(renderer,graphics.overhead_canvas,vertices,4,indices,6,mask,mask_uv);
    } else tr::draw_texture(renderer, graphics.overhead_canvas, nullptr, &rect);
}
