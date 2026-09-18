#include "renderer/device.hpp"
#include <SDL3/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

static void require(bool condition,const char* label){if(!condition){std::fprintf(stderr,"FAIL %s: %s\n",label,SDL_GetError());std::exit(1);}}
static SDL_Color pixel(SDL_Surface* s,int x,int y){SDL_Color c{};require(SDL_ReadSurfacePixel(s,x,y,&c.r,&c.g,&c.b,&c.a),"read pixel");return c;}
static void near(int a,int b,const char* label){require(std::abs(a-b)<=2,label);}
int main(){
 require(SDL_Init(SDL_INIT_VIDEO),"SDL init");
 auto* window=tr::create_window("Renderer test",256,256,SDL_WINDOW_HIDDEN);require(window,"window");
 auto* r=tr::create_renderer(window,nullptr);require(r,"renderer");
 auto* target=tr::create_texture(r,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_TARGET,128,128);require(target,"target");
 tr::set_target(r,target);tr::set_color(r,0,0,0,1);tr::clear(r);
 tr::set_scale(r,2,2);SDL_Rect clip{8,8,16,16};tr::set_clip(r,&clip);tr::set_color(r,1,0,0,1);SDL_FRect box{0,0,64,64};tr::fill_rect(r,&box);
 auto* s=tr::read_pixels(r,nullptr);require(s,"readback");near(pixel(s,20,20).r,255,"scaled clip interior");near(pixel(s,10,10).r,0,"scaled clip exterior");SDL_DestroySurface(s);
 tr::set_clip(r,nullptr);tr::set_scale(r,1,1);tr::set_color(r,0,0,1,1);tr::clear(r);tr::set_blend(r,SDL_BLENDMODE_BLEND);tr::set_color(r,1,0,0,.5F);tr::fill_rect(r,&box);
 s=tr::read_pixels(r,nullptr);require(s,"blend readback");auto c=pixel(s,16,16);near(c.r,128,"blend red");near(c.b,128,"blend blue");SDL_DestroySurface(s);
 auto* light=tr::create_texture(r,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STATIC,2,2);std::uint32_t values[]{0xffffffff,0xff000000,0xff000000,0xff000000};require(tr::update_texture(light,nullptr,values,8),"light upload");
 tr::set_blend(r,SDL_BLENDMODE_NONE);tr::set_color(r,0,0,0,1);tr::clear(r);
 SDL_Vertex vertices[]{{{0,0},{1,1,1,1},{0,0}},{{128,0},{1,1,1,1},{1,0}},{{128,128},{1,1,1,1},{1,1}},{{0,128},{1,1,1,1},{0,1}}};SDL_FPoint uv[]{{.25F,.25F},{.75F,.25F},{.75F,.75F},{.25F,.75F}};int indices[]{0,1,2,0,2,3};tr::geometry(r,r->white,vertices,4,indices,6,light,uv);
 s=tr::read_pixels(r,nullptr);require(s,"lightmap readback");near(pixel(s,63,63).r,65,"bilinear center (not triangle interpolation)");SDL_DestroySurface(s);
 tr::set_target(r,nullptr);tr::set_color(r,0,0,0,1);tr::clear(r);SDL_FRect dest{0,0,128,128};tr::draw_texture(r,target,nullptr,&dest);require(tr::present(r),"present");
 s=tr::read_pixels(r,nullptr);require(s,"target sampled readback");near(pixel(s,63,63).r,65,"target orientation");near(pixel(s,10,10).r,215,"target top left");SDL_DestroySurface(s);
 tr::destroy_renderer(r);SDL_DestroyWindow(window);SDL_Quit();std::puts("PASS renderer: clipping, scaling, alpha, bilinear lightmap, target sampling and readback");
}
