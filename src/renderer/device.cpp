#include "device.hpp"
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <cstring>
#include <cstdio>

namespace tr {
SDL_Window* create_window(const char* title,int w,int h,SDL_WindowFlags flags) {
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,0);
    flags|=SDL_WINDOW_OPENGL;
#else
    const char* driver=SDL_GetHint(SDL_HINT_VIDEO_DRIVER);
    if(!driver || std::strcmp(driver,"dummy")!=0) flags|=SDL_WINDOW_VULKAN;
#endif
    return SDL_CreateWindow(title,w,h,flags);
}
Renderer* create_renderer(SDL_Window* window,const char*) {
    auto* r=new Renderer;r->window=window;
#ifdef __EMSCRIPTEN__
    r->api=&webgl_backend();
#else
    r->api=&vulkan_backend();
#endif
    if(!r->api->init(r)){r->api->shutdown(r);delete r;return nullptr;}
    r->vertices.reserve(65536);r->indices.reserve(98304);r->commands.reserve(2048);
    r->white=create_texture(r,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,1,1);
    const std::uint32_t white=0xffffffff;
    if(!r->white || !update_texture(r->white,nullptr,&white,4)){destroy_renderer(r);return nullptr;}
    std::printf("Owned renderer: %s\n",r->api->name);
    return r;
}
void destroy_renderer(Renderer* r) {
    if(!r)return;
    flush(r);
    while(!r->textures.empty())destroy_texture(r->textures.back());
    r->api->shutdown(r);delete r;
}
bool flush(Renderer* r) {
    if(!r || r->commands.empty())return r!=nullptr;
    const bool ok=r->api->execute(r);
    r->vertices.clear();r->indices.clear();r->commands.clear();return ok;
}
Texture* create_texture(Renderer* r,SDL_PixelFormat,SDL_TextureAccess access,int w,int h) {
    if(!r || w<=0 || h<=0){SDL_SetError("Invalid texture dimensions");return nullptr;}
    auto* t=new Texture;t->owner=r;t->w=w;t->h=h;t->target=access==SDL_TEXTUREACCESS_TARGET;
    if(!r->api->create_texture(t)){delete t;return nullptr;}
    r->textures.push_back(t);return t;
}
void destroy_texture(Texture* t) {
    if(!t)return;
    auto* r=t->owner;flush(r);
    if(r->target==t)r->target=nullptr;
    r->api->destroy_texture(t);std::erase(r->textures,t);delete t;
}
Texture* texture_from_surface(Renderer* r,SDL_Surface* surface) {
    if(!surface)return nullptr;
    auto* rgba=SDL_ConvertSurface(surface,SDL_PIXELFORMAT_RGBA32);if(!rgba)return nullptr;
    auto* t=create_texture(r,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,rgba->w,rgba->h);
    if(t && !update_texture(t,nullptr,rgba->pixels,rgba->pitch)){destroy_texture(t);t=nullptr;}
    SDL_DestroySurface(rgba);return t;
}
Texture* load_texture(Renderer* r,const char* path) {
    auto* s=IMG_Load(path);if(!s)return nullptr;auto* t=texture_from_surface(r,s);SDL_DestroySurface(s);return t;
}
bool update_texture(Texture* t,const SDL_Rect* rect,const void* data,int pitch) {
    if(!t || !data)return false;
    const SDL_Rect area=rect?*rect:SDL_Rect{0,0,t->w,t->h};
    if(area.x<0 || area.y<0 || area.w<=0 || area.h<=0 || area.x+area.w>t->w || area.y+area.h>t->h || pitch<area.w*4)
        return SDL_SetError("Texture update outside image");
    if(!flush(t->owner))return false;
    ++t->owner->frame.uploads;return t->owner->api->upload(t,area,data,pitch);
}
bool texture_size(Texture* t,float* w,float* h){if(!t)return false;if(w)*w=static_cast<float>(t->w);if(h)*h=static_cast<float>(t->h);return true;}
bool texture_color(Texture* t,float r,float g,float b){if(!t)return false;t->color.r=r;t->color.g=g;t->color.b=b;return true;}
bool texture_alpha(Texture* t,float a){if(!t)return false;t->color.a=a;return true;}
bool texture_blend(Texture* t,SDL_BlendMode b){if(!t)return false;t->blend=b;return true;}
bool texture_filter(Texture* t,SDL_ScaleMode f){if(!t)return false;t->filter=f;return true;}
bool output_size(Renderer* r,int* w,int* h){if(!r)return false;if(r->target){if(w)*w=r->target->w;if(h)*h=r->target->h;return true;}return SDL_GetWindowSizeInPixels(r->window,w,h);}
bool set_target(Renderer* r,Texture* t){if(!r || (t && (!t->target || t->owner!=r)))return SDL_SetError("Invalid render target");r->target=t;return true;}
Texture* get_target(Renderer* r){return r?r->target:nullptr;}
static ViewState& view(Renderer* r){return r->target?r->target->view:r->window_view;}
bool set_scale(Renderer* r,float x,float y){if(!r || x<=0 || y<=0)return false;view(r).sx=x;view(r).sy=y;return true;}
bool get_scale(Renderer* r,float* x,float* y){if(!r)return false;if(x)*x=view(r).sx;if(y)*y=view(r).sy;return true;}
bool set_viewport(Renderer* r,const SDL_Rect* rect){if(!r)return false;view(r).viewport_set=rect!=nullptr;if(rect)view(r).viewport=*rect;return true;}
bool get_viewport(Renderer* r,SDL_Rect* rect){if(!r || !rect)return false;auto& v=view(r);if(v.viewport_set)*rect=v.viewport;else{int w,h;output_size(r,&w,&h);*rect={0,0,static_cast<int>(static_cast<float>(w)/v.sx),static_cast<int>(static_cast<float>(h)/v.sy)};}return true;}
bool viewport_set(Renderer* r){return r&&view(r).viewport_set;}
bool set_clip(Renderer* r,const SDL_Rect* rect){if(!r)return false;view(r).clip_set=rect!=nullptr;if(rect)view(r).clip=*rect;return true;}
bool get_clip(Renderer* r,SDL_Rect* rect){if(!r||!rect)return false;*rect=view(r).clip_set?view(r).clip:SDL_Rect{};return true;}
bool clip_enabled(Renderer* r){return r&&view(r).clip_set;}
bool set_color(Renderer* r,float a,float b,float c,float d){if(!r)return false;r->color={a,b,c,d};return true;}
bool get_color(Renderer* r,float* a,float* b,float* c,float* d){if(!r)return false;if(a)*a=r->color.r;if(b)*b=r->color.g;if(c)*c=r->color.b;if(d)*d=r->color.a;return true;}
bool set_blend(Renderer* r,SDL_BlendMode b){if(!r)return false;r->blend=b;return true;}
bool get_blend(Renderer* r,SDL_BlendMode* b){if(!r||!b)return false;*b=r->blend;return true;}
bool clear(Renderer* r){if(!r)return false;Command c;c.target=r->target;c.clear=true;c.color=r->color;r->commands.push_back(c);return true;}
bool present(Renderer* r){if(!flush(r))return false;const bool ok=r->api->present(r);r->last=r->frame;r->frame={};return ok;}
bool set_vsync(Renderer* r,int v){if(!r||v<0||v>1)return false;r->vsync=v;return true;}
bool get_vsync(Renderer* r,int* v){if(!r||!v)return false;*v=r->vsync;return true;}
const char* renderer_name(Renderer* r){return r?r->api->name:"none";}
SDL_Surface* read_pixels(Renderer* r,const SDL_Rect* rect){if(!flush(r))return nullptr;int w,h;output_size(r,&w,&h);return r->api->read(r,r->target,rect?*rect:SDL_Rect{0,0,w,h});}
}
