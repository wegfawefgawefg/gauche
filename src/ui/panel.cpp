#include "panel.hpp"

// SHAPE: The same lean and offset shadow frame the inventory, quick slots and vitals.
void angled_fill(tr::Renderer* renderer, SDL_FRect rect, SDL_FColor top, SDL_FColor bottom) {
    const float x = rect.x, y = rect.y, width = rect.w, height = rect.h;
    const SDL_Vertex vertices[4]{
        {{x+4,y},top,{}}, {{x+width+4,y},top,{}},
        {{x+width,y+height},bottom,{}}, {{x,y+height},bottom,{}}};
    constexpr int indices[]{0,1,2,0,2,3};
    tr::geometry(renderer, nullptr, vertices, 4, indices, 6);
}

void angled_panel(tr::Renderer* renderer, SDL_FRect rect, SDL_Color edge) {
    tr::set_blend(renderer, SDL_BLENDMODE_BLEND);
    const SDL_FRect shadow{rect.x+3,rect.y+4,rect.w,rect.h};
    angled_fill(renderer, shadow, {0,0,0,.75F}, {0,0,0,.75F});
    angled_fill(renderer, rect, {.10F,.10F,.10F,.98F}, {.06F,.07F,.07F,.98F});
    tr::set_color_bytes(renderer,edge.r,edge.g,edge.b,edge.a);
    const SDL_FPoint points[]{{rect.x+4,rect.y},{rect.x+rect.w+4,rect.y},
        {rect.x+rect.w,rect.y+rect.h},{rect.x,rect.y+rect.h},{rect.x+4,rect.y}};
    tr::lines(renderer,points,5);
    tr::set_blend(renderer,SDL_BLENDMODE_NONE);
}
