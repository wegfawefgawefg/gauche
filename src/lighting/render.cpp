#include "render.hpp"

#include <algorithm>
#include <array>

namespace {

SDL_FColor vertex_color(LightColor light, LightColor tint) {
  return {std::clamp(light.red * tint.red, 0.0F, 1.0F),
          std::clamp(light.green * tint.green, 0.0F, 1.0F),
          std::clamp(light.blue * tint.blue, 0.0F, 1.0F), 1.0F};
}

// Each texel stores an existing corner sample. The shader interpolates those
// samples across the square, independently of its two geometry triangles.
tr::Texture *lightmap(tr::Renderer *renderer, const LightingCache &lighting) {
  if (renderer->light_source == &lighting &&
      renderer->light_revision == lighting.revision)
    return renderer->lightmap;
  const int width = lighting.width + 1, height = lighting.height + 1;
  auto *&texture = renderer->lightmap;
  if (texture && (texture->w != width || texture->h != height)) {
    tr::destroy_texture(texture);
    texture = nullptr;
  }
  if (!texture)
    texture = tr::create_texture(renderer, SDL_PIXELFORMAT_RGBA32,
                                 SDL_TEXTUREACCESS_STATIC, width, height);
  if (!texture)
    return nullptr;
  std::vector<std::uint8_t> pixels(
      static_cast<std::size_t>(width * height * 4));
  const auto byte = [](float x) {
    return static_cast<std::uint8_t>(std::clamp(x, 0.0F, 1.0F) * 255.0F + .5F);
  };
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      const auto color =
          light_at_corner(lighting, lighting.origin + Cell{x, y});
      const auto offset = static_cast<std::size_t>((y * width + x) * 4);
      pixels[offset] = byte(color.red);
      pixels[offset + 1] = byte(color.green);
      pixels[offset + 2] = byte(color.blue);
      pixels[offset + 3] = 255;
    }
  if (!tr::update_texture(texture, nullptr, pixels.data(), width * 4))
    return nullptr;
  renderer->light_source = &lighting;
  renderer->light_revision = lighting.revision;
  return texture;
}

} // namespace

void draw_flat_tile(tr::Renderer *renderer, tr::Texture *texture,
                    SDL_FRect rect, SDL_FColor color, SDL_FRect uv,
                    int quarter_turns, bool flip_horizontal) {
  constexpr std::array<SDL_FPoint, 4> points{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
  std::array<SDL_Vertex, 4> vertices{};
  for (std::size_t i = 0; i < points.size(); ++i) {
    const auto p = points[i];
    float u = p.x, v = p.y;
    for (int turn = 0; turn < (quarter_turns & 3); ++turn) {
      const float old = u;
      u = v;
      v = 1 - old;
    }
    if (flip_horizontal)
      u = 1 - u;
    vertices[i] = {{rect.x + rect.w * p.x, rect.y + rect.h * p.y},
                   color,
                   {uv.x + uv.w * u, uv.y + uv.h * v}};
  }
  constexpr int indices[]{0, 1, 2, 0, 2, 3};
  tr::geometry(renderer, texture, vertices.data(),
               static_cast<int>(vertices.size()), indices, 6);
}

void draw_lit_tile(tr::Renderer *renderer, tr::Texture *texture, SDL_FRect rect,
                   Cell cell, const LightingCache &lighting, LightColor tint,
                   SDL_FRect uv, int quarter_turns, float opacity,
                   bool flip_horizontal) {
  if (texture == nullptr)
    return;
  if (!lighting.active) {
    auto color = vertex_color({1, 1, 1}, tint);
    color.a = opacity;
    draw_flat_tile(renderer, texture, rect, color, uv, quarter_turns,
                   flip_horizontal);
    return;
  }
  tr::Texture *map =
      renderer->smooth_lighting ? lightmap(renderer, lighting) : nullptr;
  constexpr std::array<SDL_FPoint, 4> points{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}};
  std::array<SDL_FColor, 4> colors{};
  if (!map) {
    constexpr Cell offsets[]{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    for (std::size_t i = 0; i < colors.size(); ++i)
      colors[i] =
          vertex_color(light_at_corner(lighting, cell + offsets[i]), tint);
  }
  std::array<SDL_Vertex, 4> vertices{};
  std::array<SDL_FPoint, 4> light_uv{};
  for (std::size_t i = 0; i < points.size(); ++i) {
    const auto p = points[i];
    float u = p.x, v = p.y;
    for (int turn = 0; turn < (quarter_turns & 3); ++turn) {
      const float old = u;
      u = v;
      v = 1 - old;
    }
    if (flip_horizontal)
      u = 1 - u;
    auto color = map ? vertex_color({1, 1, 1}, tint) : colors[i];
    if (map)
      light_uv[i] = {
          (static_cast<float>(cell.x - lighting.origin.x) + p.x + .5F) /
              static_cast<float>(map->w),
          (static_cast<float>(cell.y - lighting.origin.y) + p.y + .5F) /
              static_cast<float>(map->h)};
    color.a = opacity;
    vertices[i] = {{rect.x + rect.w * p.x, rect.y + rect.h * p.y},
                   color,
                   {uv.x + uv.w * u, uv.y + uv.h * v}};
  }
  constexpr std::array<int, 6> indices{0, 1, 2, 0, 2, 3};
  tr::geometry(renderer, texture, vertices.data(),
               static_cast<int>(vertices.size()), indices.data(),
               static_cast<int>(indices.size()), map,
               map ? light_uv.data() : nullptr);
}

LightColor lit_sprite_color(const LightingCache &lighting, Cell cell,
                            LightColor self_glow) {
  const LightColor local = light_at_cell(lighting, cell);
  return {std::max(local.red, self_glow.red),
          std::max(local.green, self_glow.green),
          std::max(local.blue, self_glow.blue)};
}
