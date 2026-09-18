#pragma once
#include <SDL3/SDL.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct TeemingRenderer;
struct TeemingTexture;
namespace tr {
using Renderer = ::TeemingRenderer;
using Texture = ::TeemingTexture;
struct Vertex {
  float x = 0, y = 0, z = 0;
  float r = 1, g = 1, b = 1, a = 1;
  float u = 0, v = 0, lu = 0, lv = 0;
};
struct ViewState {
  float sx = 1, sy = 1;
  SDL_Rect viewport{};
  SDL_Rect clip{};
  bool viewport_set = false, clip_set = false;
};
} // namespace tr
struct TeemingTexture {
  tr::Renderer *owner = nullptr;
  int w = 0, h = 0;
  bool target = false;
  SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA32;
  SDL_FColor color{1, 1, 1, 1};
  SDL_BlendMode blend = SDL_BLENDMODE_BLEND;
  SDL_ScaleMode filter = SDL_SCALEMODE_NEAREST;
  tr::ViewState view;
  void *gpu = nullptr;
  tr::Texture *storage = nullptr;
  int atlas_x = 0, atlas_y = 0;
};
namespace tr {
struct Command {
  Texture *target = nullptr;
  Texture *texture = nullptr;
  Texture *light = nullptr;
  SDL_BlendMode blend = SDL_BLENDMODE_BLEND;
  SDL_ScaleMode filter = SDL_SCALEMODE_NEAREST;
  SDL_Rect clip{};
  std::uint32_t first = 0, count = 0;
  bool clear = false;
  SDL_FColor color{};
};
struct AtlasPage {
  Texture *texture = nullptr;
  int x = 0, y = 0, row = 0;
};
struct Stats {
  std::uint64_t vertices = 0, triangles = 0, batches = 0, uploads = 0;
};
struct Backend {
  const char *name;
  bool (*init)(Renderer *);
  void (*shutdown)(Renderer *);
  bool (*create_texture)(Texture *);
  void (*destroy_texture)(Texture *);
  bool (*upload)(Texture *, const SDL_Rect &, const void *, int);
  bool (*execute)(Renderer *);
  bool (*present)(Renderer *);
  SDL_Surface *(*read)(Renderer *, Texture *, const SDL_Rect &);
};
} // namespace tr
struct TeemingRenderer {
  SDL_Window *window = nullptr;
  const tr::Backend *api = nullptr;
  void *gpu = nullptr;
  tr::Texture *target = nullptr;
  tr::Texture *white = nullptr;
  tr::Texture *font = nullptr;
  tr::Texture *lightmap = nullptr;
  // Local presentation only: flat cells, triangle interpolation, bilinear lightmap.
  int lighting_style = 2;
  const void *light_source = nullptr;
  std::uint64_t light_revision = 0;
  std::vector<tr::AtlasPage> atlases;
  std::vector<tr::Texture *> textures;
  tr::ViewState window_view;
  SDL_FColor color{1, 1, 1, 1};
  SDL_BlendMode blend = SDL_BLENDMODE_NONE;
  int vsync = 1;
  std::vector<tr::Vertex> vertices;
  std::vector<std::uint32_t> indices;
  std::vector<tr::Command> commands;
  tr::Stats frame, last;
  std::string error;
};
namespace tr {
const Backend &vulkan_backend();
const Backend &webgl_backend();
const Backend &webgpu_backend();
bool webgpu_available();
bool flush(Renderer *);
bool geometry(Renderer *, Texture *, const SDL_Vertex *, int, const int *, int,
              Texture *light = nullptr, const SDL_FPoint *light_uv = nullptr);
void submit(Renderer *, Texture *, Texture *, SDL_BlendMode, const Vertex *,
            int, const int *, int);
SDL_Window *create_window(const char *, int, int, SDL_WindowFlags);
Renderer *create_renderer(SDL_Window *, const char *);
void destroy_renderer(Renderer *);
Texture *create_texture(Renderer *, SDL_PixelFormat, SDL_TextureAccess, int,
                        int);
Texture *texture_from_surface(Renderer *, SDL_Surface *);
Texture *load_texture(Renderer *, const char *);
Texture *atlas_texture(Renderer *, SDL_Surface *);
void destroy_texture(Texture *);
bool update_texture(Texture *, const SDL_Rect *, const void *, int);
bool texture_size(Texture *, float *, float *);
bool texture_color(Texture *, float, float, float);
bool texture_alpha(Texture *, float);
bool texture_blend(Texture *, SDL_BlendMode);
bool texture_filter(Texture *, SDL_ScaleMode);
bool output_size(Renderer *, int *, int *);
bool set_target(Renderer *, Texture *);
Texture *get_target(Renderer *);
bool set_scale(Renderer *, float, float);
bool get_scale(Renderer *, float *, float *);
bool set_viewport(Renderer *, const SDL_Rect *);
bool get_viewport(Renderer *, SDL_Rect *);
bool viewport_set(Renderer *);
bool set_clip(Renderer *, const SDL_Rect *);
bool get_clip(Renderer *, SDL_Rect *);
bool clip_enabled(Renderer *);
bool set_color(Renderer *, float, float, float, float);
bool get_color(Renderer *, float *, float *, float *, float *);
bool set_blend(Renderer *, SDL_BlendMode);
bool get_blend(Renderer *, SDL_BlendMode *);
bool clear(Renderer *);
bool present(Renderer *);
bool set_vsync(Renderer *, int);
bool get_vsync(Renderer *, int *);
const char *renderer_name(Renderer *);
bool fill_rect(Renderer *, const SDL_FRect *);
bool fill_rects(Renderer *, const SDL_FRect *, int);
bool line(Renderer *, float, float, float, float);
bool lines(Renderer *, const SDL_FPoint *, int);
bool point(Renderer *, float, float);
bool rect(Renderer *, const SDL_FRect *);
bool draw_texture(Renderer *, Texture *, const SDL_FRect *, const SDL_FRect *);
bool draw_rotated(Renderer *, Texture *, const SDL_FRect *, const SDL_FRect *,
                  double, const SDL_FPoint *, SDL_FlipMode);
bool raw_geometry(Renderer *, Texture *, const float *, int, const SDL_FColor *,
                  int, const float *, int, int, const void *, int, int);
bool debug_text(Renderer *, float, float, const char *);
SDL_Surface *read_pixels(Renderer *, const SDL_Rect *);

inline bool set_color_bytes(Renderer *r, Uint8 red, Uint8 green, Uint8 blue,
                            Uint8 alpha) {
  return set_color(r, red / 255.F, green / 255.F, blue / 255.F, alpha / 255.F);
}
inline bool texture_color_bytes(Texture *t, Uint8 red, Uint8 green,
                                Uint8 blue) {
  return texture_color(t, red / 255.F, green / 255.F, blue / 255.F);
}
inline bool texture_alpha_bytes(Texture *t, Uint8 alpha) {
  return texture_alpha(t, alpha / 255.F);
}
} // namespace tr
