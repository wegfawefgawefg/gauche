#include "drummer_render.hpp"
#include "whiteout_drummer.hpp"
#include "../lighting/render.hpp"

#include <algorithm>

void draw_drummer_warning(SDL_Renderer* renderer, const Game& game, const Entity& drummer,
    ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (drummer.health <= 0 || drummer.label_a != DrummerBeats || drummer.cell != drummer.point_a) return;
    const SDL_FRect rect = tile_rect(drummer.point_b,camera,zoom);
    const LightColor light = light_at_cell(lighting,drummer.point_b);
    const float age = static_cast<float>((game.tick % 24))/24;
    const float radius = rect.w*(.4F+age*.3F);
    const float size = rect.w/16;
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColorFloat(renderer,light.red*.8F,light.green*.9F,light.blue,
        .4F+static_cast<float>(std::clamp(drummer.counter_a,1,3))*.16F);
    // TELL: Four drifting flecks at the committed center; no permanent attack grid.
    for (Cell side : {Cell{1,0},{0,1},{-1,0},{0,-1}}) {
        SDL_FRect flake{rect.x+rect.w*.5F+static_cast<float>(side.x)*radius,
            rect.y+rect.h*.5F+static_cast<float>(side.y)*radius,size,size};
        SDL_RenderFillRect(renderer,&flake);
    }
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_NONE);
}
