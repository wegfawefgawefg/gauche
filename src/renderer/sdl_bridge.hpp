#pragma once
// Dependency migration boundary: Gubsy/GView/ImGui keep their public drawing
// vocabulary while their renderer and textures are owned by Teeming. SDL itself
// and the image/font decoders compile without this header.
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "device.hpp"
#define SDL_Renderer TeemingRenderer
#define SDL_Texture TeemingTexture
#define SDL_CreateWindow tr::create_window
#define SDL_CreateRenderer tr::create_renderer
#define SDL_DestroyRenderer tr::destroy_renderer
#define SDL_CreateTexture tr::create_texture
#define SDL_CreateTextureFromSurface tr::texture_from_surface
#define IMG_LoadTexture tr::load_texture
#define SDL_DestroyTexture tr::destroy_texture
#define SDL_UpdateTexture tr::update_texture
#define SDL_GetTextureSize tr::texture_size
#define SDL_SetTextureColorModFloat tr::texture_color
#define SDL_SetTextureColorMod(t,r,g,b) tr::texture_color(t,(r)/255.F,(g)/255.F,(b)/255.F)
#define SDL_SetTextureAlphaModFloat tr::texture_alpha
#define SDL_SetTextureAlphaMod(t,a) tr::texture_alpha(t,(a)/255.F)
#define SDL_SetTextureBlendMode tr::texture_blend
#define SDL_SetTextureScaleMode tr::texture_filter
#define SDL_GetCurrentRenderOutputSize tr::output_size
#define SDL_SetRenderTarget tr::set_target
#define SDL_GetRenderTarget tr::get_target
#define SDL_SetRenderScale tr::set_scale
#define SDL_GetRenderScale tr::get_scale
#define SDL_SetRenderViewport tr::set_viewport
#define SDL_GetRenderViewport tr::get_viewport
#define SDL_RenderViewportSet tr::viewport_set
#define SDL_SetRenderClipRect tr::set_clip
#define SDL_GetRenderClipRect tr::get_clip
#define SDL_RenderClipEnabled tr::clip_enabled
#define SDL_SetRenderDrawColorFloat tr::set_color
#define SDL_SetRenderDrawColor(r,a,b,c,d) tr::set_color(r,(a)/255.F,(b)/255.F,(c)/255.F,(d)/255.F)
#define SDL_GetRenderDrawColorFloat tr::get_color
#define SDL_SetRenderDrawBlendMode tr::set_blend
#define SDL_GetRenderDrawBlendMode tr::get_blend
#define SDL_RenderClear tr::clear
#define SDL_RenderPresent tr::present
#define SDL_SetRenderVSync tr::set_vsync
#define SDL_GetRenderVSync tr::get_vsync
#define SDL_GetRendererName tr::renderer_name
#define SDL_RenderFillRect tr::fill_rect
#define SDL_RenderFillRects tr::fill_rects
#define SDL_RenderRect tr::rect
#define SDL_RenderLine tr::line
#define SDL_RenderLines tr::lines
#define SDL_RenderPoint tr::point
#define SDL_RenderTexture tr::draw_texture
#define SDL_RenderTextureRotated tr::draw_rotated
#define SDL_RenderGeometry tr::geometry
#define SDL_RenderGeometryRaw tr::raw_geometry
#define SDL_RenderDebugText tr::debug_text
#define SDL_RenderReadPixels tr::read_pixels
