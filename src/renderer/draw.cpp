#include "device.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace tr {
void submit(Renderer* r,Texture* texture,Texture* light,SDL_BlendMode blend,const Vertex* vertices,int count,const int* indices,int index_count) {
    if(!r || count<=0)return;
    if(texture==r->target && texture){SDL_SetError("Cannot sample the active render target");return;}
    const auto& v=r->target?r->target->view:r->window_view;
    int w=0,h=0;output_size(r,&w,&h);
    SDL_Rect viewport{0,0,w,h};
    if(v.viewport_set)viewport={static_cast<int>(v.viewport.x*v.sx),static_cast<int>(v.viewport.y*v.sy),static_cast<int>(v.viewport.w*v.sx),static_cast<int>(v.viewport.h*v.sy)};
    SDL_Rect clip=viewport;
    if(v.clip_set){SDL_Rect requested{viewport.x+static_cast<int>(v.clip.x*v.sx),viewport.y+static_cast<int>(v.clip.y*v.sy),static_cast<int>(v.clip.w*v.sx),static_cast<int>(v.clip.h*v.sy)};if(!SDL_GetRectIntersection(&viewport,&requested,&clip))return;}
    SDL_Rect screen{0,0,w,h};if(!SDL_GetRectIntersection(&screen,&clip,&clip))return;
    const auto base=static_cast<std::uint32_t>(r->vertices.size());
    for(int i=0;i<count;++i){auto p=vertices[i];p.x=p.x*v.sx+viewport.x;p.y=p.y*v.sy+viewport.y;r->vertices.push_back(p);}
    Command c;c.target=r->target;c.texture=texture?texture:r->white;c.light=light?light:r->white;c.blend=blend;c.filter=texture?texture->filter:SDL_SCALEMODE_NEAREST;c.clip=clip;c.first=static_cast<std::uint32_t>(r->indices.size());c.count=static_cast<std::uint32_t>(indices?index_count:count);
    for(std::uint32_t i=0;i<c.count;++i)r->indices.push_back(base+static_cast<std::uint32_t>(indices?indices[i]:static_cast<int>(i)));
    r->frame.vertices+=static_cast<std::uint64_t>(count);r->frame.triangles+=c.count/3;
    if(!r->commands.empty()){
        auto& b=r->commands.back();
        if(!b.clear && b.target==c.target && b.texture==c.texture && b.light==c.light && b.filter==c.filter && b.blend==c.blend && std::memcmp(&b.clip,&c.clip,sizeof(c.clip))==0 && b.first+b.count==c.first){b.count+=c.count;return;}
    }
    r->commands.push_back(c);++r->frame.batches;
}
bool geometry(Renderer* r,Texture* t,const SDL_Vertex* vertices,int n,const int* indices,int ni,Texture* light,const SDL_FPoint* light_uv){
    if(!r || !vertices || n<0 || ni<0)return false;
    std::vector<Vertex> data;data.reserve(static_cast<std::size_t>(n));
    for(int i=0;i<n;++i){const auto& p=vertices[i];const auto uv=light_uv?light_uv[i]:SDL_FPoint{};data.push_back({p.position.x,p.position.y,0,p.color.r,p.color.g,p.color.b,p.color.a,p.tex_coord.x,p.tex_coord.y,uv.x,uv.y});}
    for(int i=0;i<ni;++i)if(indices && (indices[i]<0 || indices[i]>=n))return SDL_SetError("Geometry index outside vertex array");
    submit(r,t,light,t?t->blend:r->blend,data.data(),n,indices,ni);return true;
}
bool raw_geometry(Renderer* r,Texture* t,const float* xy,int xs,const SDL_FColor* colors,int cs,const float* uv,int us,int n,const void* indices,int ni,int is){
    if(n<0||ni<0||!xy||!colors)return false;
    std::vector<SDL_Vertex> vertices(static_cast<std::size_t>(n));std::vector<int> idx(static_cast<std::size_t>(ni));
    for(int i=0;i<n;++i){auto& v=vertices[static_cast<std::size_t>(i)];std::memcpy(&v.position,reinterpret_cast<const char*>(xy)+i*xs,sizeof(v.position));std::memcpy(&v.color,reinterpret_cast<const char*>(colors)+i*cs,sizeof(v.color));if(uv)std::memcpy(&v.tex_coord,reinterpret_cast<const char*>(uv)+i*us,sizeof(v.tex_coord));}
    for(int i=0;indices&&i<ni;++i){std::uint32_t value=0;if(is!=1&&is!=2&&is!=4)return false;std::memcpy(&value,static_cast<const char*>(indices)+i*is,static_cast<std::size_t>(is));if(value>=static_cast<std::uint32_t>(n))return false;idx[static_cast<std::size_t>(i)]=static_cast<int>(value);}
    return geometry(r,t,vertices.data(),n,indices?idx.data():nullptr,ni);
}
bool fill_rect(Renderer* r,const SDL_FRect* requested){
    if(!r)return false;SDL_Rect full{};get_viewport(r,&full);const SDL_FRect box=requested?*requested:SDL_FRect{0,0,static_cast<float>(full.w),static_cast<float>(full.h)};
    if(box.w<=0||box.h<=0)return true;
    SDL_Vertex v[4]{{{box.x,box.y},r->color,{}},{{box.x+box.w,box.y},r->color,{}},{{box.x+box.w,box.y+box.h},r->color,{}},{{box.x,box.y+box.h},r->color,{}}};constexpr int idx[]{0,1,2,0,2,3};return geometry(r,nullptr,v,4,idx,6);
}
bool fill_rects(Renderer* r,const SDL_FRect* rects,int n){for(int i=0;i<n;++i)if(!fill_rect(r,&rects[i]))return false;return true;}
bool point(Renderer* r,float x,float y){float sx,sy;get_scale(r,&sx,&sy);SDL_FRect rect{x,y,1/sx,1/sy};return fill_rect(r,&rect);}
bool line(Renderer* r,float x,float y,float ex,float ey){
    if(!r)return false;float sx,sy;get_scale(r,&sx,&sy);const float dx=(ex-x)*sx,dy=(ey-y)*sy,len=std::hypot(dx,dy);if(len<.01F)return point(r,x,y);
    const float nx=-dy/len*.5F/sx,ny=dx/len*.5F/sy;
    SDL_Vertex v[4]{{{x+nx+.5F/sx,y+ny+.5F/sy},r->color,{}},{{ex+nx+.5F/sx,ey+ny+.5F/sy},r->color,{}},{{ex-nx+.5F/sx,ey-ny+.5F/sy},r->color,{}},{{x-nx+.5F/sx,y-ny+.5F/sy},r->color,{}}};constexpr int idx[]{0,1,2,0,2,3};return geometry(r,nullptr,v,4,idx,6);
}
bool lines(Renderer* r,const SDL_FPoint* p,int n){for(int i=1;i<n;++i)if(!line(r,p[i-1].x,p[i-1].y,p[i].x,p[i].y))return false;return true;}
bool rect(Renderer* r,const SDL_FRect* p){SDL_Rect full;get_viewport(r,&full);SDL_FRect b=p?*p:SDL_FRect{0,0,static_cast<float>(full.w),static_cast<float>(full.h)};SDL_FPoint points[]{{b.x,b.y},{b.x+b.w-1,b.y},{b.x+b.w-1,b.y+b.h-1},{b.x,b.y+b.h-1},{b.x,b.y}};return lines(r,points,5);}
bool draw_texture(Renderer* r,Texture* t,const SDL_FRect* src,const SDL_FRect* dst){return draw_rotated(r,t,src,dst,0,nullptr,SDL_FLIP_NONE);}
bool draw_rotated(Renderer* r,Texture* t,const SDL_FRect* src,const SDL_FRect* dst,double angle,const SDL_FPoint* pivot,SDL_FlipMode flip){
    if(!r||!t)return false;SDL_Rect full;get_viewport(r,&full);
    SDL_FRect s=src?*src:SDL_FRect{0,0,static_cast<float>(t->w),static_cast<float>(t->h)},d=dst?*dst:SDL_FRect{0,0,static_cast<float>(full.w),static_cast<float>(full.h)};
    if(d.w<=0||d.h<=0)return true;
    const SDL_FPoint center=pivot?*pivot:SDL_FPoint{d.w*.5F,d.h*.5F};const double rad=angle*3.14159265358979323846/180.;const float c=static_cast<float>(std::cos(rad)),sn=static_cast<float>(std::sin(rad));
    constexpr SDL_FPoint points[]{{0,0},{1,0},{1,1},{0,1}};SDL_Vertex v[4];
    for(int i=0;i<4;++i){auto p=points[i];const float x=p.x*d.w-center.x,y=p.y*d.h-center.y;float u=p.x,w=p.y;if(flip&SDL_FLIP_HORIZONTAL)u=1-u;if(flip&SDL_FLIP_VERTICAL)w=1-w;v[i]={{d.x+center.x+x*c-y*sn,d.y+center.y+x*sn+y*c},t->color,{(s.x+u*s.w)/t->w,(s.y+w*s.h)/t->h}};}
    constexpr int idx[]{0,1,2,0,2,3};return geometry(r,t,v,4,idx,6);
}
}
