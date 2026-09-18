#include "cutter_render.hpp"
#include "coal_cutter.hpp"

void draw_cutter_details(tr::Renderer* renderer,const GameGraphics& graphics,const Entity& cutter,
    SDL_FRect rect,LightColor light) {
    if (cutter.kind!=EntityKind::CoalCutter || cutter.health<=0) return;
    const bool working=cutter.label_a==CutterWorking && cutter.stun_ticks==0 && cutter.sleep_ticks==0;
    const float pixel=rect.w/16,dx=static_cast<float>(cutter.facing.x),dy=static_cast<float>(cutter.facing.y);
    const float reach=working ? .55F+.17F*(1-static_cast<float>(cutter.timer_a)/60) : .38F;
    const float cx=rect.x+rect.w*.5F,cy=rect.y+rect.h*.5F;
    tr::set_color(renderer,light.red*.6F,light.green*.63F,light.blue*.55F,1);
    for (int w=-1;w<=1;++w) tr::line(renderer,cx-dy*pixel*static_cast<float>(w),cy+dx*pixel*static_cast<float>(w),
        cx+dx*rect.w*reach-dy*pixel*static_cast<float>(w),cy+dy*rect.h*reach+dx*pixel*static_cast<float>(w));
    tr::Texture* teeth=texture_for(graphics,Sprite::CutterTeeth);
    tr::texture_color(teeth,light.red,light.green,light.blue);
    SDL_FRect head{cx+dx*rect.w*reach-rect.w*.25F,cy+dy*rect.h*reach-rect.h*.25F,rect.w*.5F,rect.h*.5F};
    const double angle=cutter.facing.x>0 ? 0 : cutter.facing.x<0 ? 180 : cutter.facing.y>0 ? 90 : -90;
    tr::draw_rotated(renderer,teeth,nullptr,&head,angle,nullptr,
        working && cutter.timer_a/4%2 ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
    tr::texture_color(teeth,1,1,1);
    // Shape as well as color distinguishes a jammed cross from a running lamp.
    SDL_FRect lamp{rect.x+pixel*3,rect.y+pixel*3,pixel*3,pixel*2};
    const LightColor color=cutter.label_a==CutterBlocked ? LightColor{.9F,.32F,.15F} :
        working ? LightColor{.8F,.7F,.25F} : LightColor{.22F,.26F,.23F};
    tr::set_color(renderer,light.red*color.red,light.green*color.green,light.blue*color.blue,1);
    tr::fill_rect(renderer,&lamp);
    if (cutter.label_a==CutterUnpowered) {
        for (int x:{8,11}) tr::line(renderer,rect.x+pixel*static_cast<float>(x),rect.y+pixel*6,
            rect.x+pixel*static_cast<float>(x),rect.y+pixel*9);
    }
    if (cutter.label_a==CutterBlocked) {
        tr::line(renderer,rect.x+pixel*8,rect.y+pixel*6,rect.x+pixel*11,rect.y+pixel*9);
        tr::line(renderer,rect.x+pixel*11,rect.y+pixel*6,rect.x+pixel*8,rect.y+pixel*9);
    }
}
