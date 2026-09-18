#include "device.hpp"
#include <algorithm>
#include <cstring>
namespace tr {
Texture *atlas_texture(Renderer *r, SDL_Surface *source) {
  // Only immutable file assets enter atlases. Transient text and render targets
  // retain independent lifetimes and can be destroyed without fragmenting
  // pages.
  constexpr int page_size = 2048;
  if (source->w > 512 || source->h > 512)
    return texture_from_surface(r, source);
  auto *rgba = SDL_ConvertSurface(source, SDL_PIXELFORMAT_RGBA32);
  if (!rgba)
    return nullptr;
  const int width = rgba->w + 2, height = rgba->h + 2;
  AtlasPage *page = nullptr;
  for (auto &candidate : r->atlases) {
    if (candidate.x + width > page_size &&
        candidate.y + candidate.row + height <= page_size) {
      candidate.y += candidate.row;
      candidate.x = 0;
      candidate.row = 0;
    }
    if (candidate.x + width <= page_size && candidate.y + height <= page_size) {
      page = &candidate;
      break;
    }
  }
  if (!page) {
    auto *storage =
        create_texture(r, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                       page_size, page_size);
    if (!storage) {
      SDL_DestroySurface(rgba);
      return nullptr;
    }
    r->atlases.push_back({storage});
    page = &r->atlases.back();
  }
  std::vector<std::uint8_t> padded(
      static_cast<std::size_t>(width * height * 4));
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      const int sx = std::clamp(x - 1, 0, rgba->w - 1),
                sy = std::clamp(y - 1, 0, rgba->h - 1);
      std::memcpy(padded.data() + static_cast<std::size_t>((y * width + x) * 4),
                  static_cast<const char *>(rgba->pixels) + sy * rgba->pitch +
                      sx * 4,
                  4);
    }
  SDL_Rect area{page->x, page->y, width, height};
  if (!update_texture(page->texture, &area, padded.data(), width * 4)) {
    SDL_DestroySurface(rgba);
    return nullptr;
  }
  auto *texture = new Texture;
  texture->owner = r;
  texture->w = rgba->w;
  texture->h = rgba->h;
  texture->storage = page->texture;
  texture->atlas_x = page->x + 1;
  texture->atlas_y = page->y + 1;
  page->x += width;
  page->row = std::max(page->row, height);
  r->textures.push_back(texture);
  SDL_DestroySurface(rgba);
  return texture;
}
} // namespace tr
