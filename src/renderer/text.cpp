#include "device.hpp"
#include "debug_font_data.hpp"
namespace tr {
bool debug_text(Renderer* r,float x,float y,const char* text){
    if(!r||!text)return false;
    if(!r->font){
        auto* surface=SDL_CreateSurface(128,128,SDL_PIXELFORMAT_RGBA32);if(!surface)return false;
        SDL_ClearSurface(surface,0,0,0,0);
        for(int ch=0;ch<SDL_DEBUG_FONT_NUM_GLYPHS;++ch)for(int row=0;row<8;++row)for(int col=0;col<8;++col){
            auto* out=reinterpret_cast<std::uint32_t*>(static_cast<char*>(surface->pixels)+(ch/16*8+row)*surface->pitch);
            out[ch%16*8+col]=(SDL_RenderDebugTextFontData[ch*8+row]&(1<<col))?0xffffffff:0;
        }
        r->font=texture_from_surface(r,surface);SDL_DestroySurface(surface);if(!r->font)return false;
    }
    r->font->color=r->color;const float start=x;
    for(const unsigned char* p=reinterpret_cast<const unsigned char*>(text);*p;++p){if(*p=='\n'){x=start;y+=8;continue;}if(*p=='\t'){x+=32;continue;}if(*p<33){x+=8;continue;}const int ch=*p<127?*p-33:SDL_DEBUG_FONT_NUM_GLYPHS-1;SDL_FRect src{static_cast<float>(ch%16*8),static_cast<float>(ch/16*8),8,8},dst{x,y,8,8};draw_texture(r,r->font,&src,&dst);x+=8;}
    return true;
}
}
