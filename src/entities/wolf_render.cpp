#include "wolf_render.hpp"
#include "wolf_call.hpp"
#include "wolf.hpp"
#include "hearing.hpp"

void pose_wolf(const Entity& wolf,SDL_FRect& rect,double& angle) {
    if (wolf.kind!=EntityKind::Wolf || wolf.label_a==WolfHunt) return;
    angle=wolf.facing.y>0 ? 90 : wolf.facing.y<0 ? -90 : 0;
    const float lean=wolf.label_a==WolfBiteWindup ? -.07F : wolf.use_flash>0 ? .12F : 0;
    rect.x+=rect.w*lean*static_cast<float>(wolf.facing.x);
    rect.y+=rect.h*lean*static_cast<float>(wolf.facing.y);
}

void draw_wolf_call(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Entity& wolf, SDL_FRect rect, LightColor light) {
    if (wolf.kind != EntityKind::Wolf || wolf.health <= 0 || wolf.label_c != WhistleHunt || wolf.timer_c <= 0) return;
    // CALL: A small musical mark and draining beat show the temporary distraction.
    SDL_Texture* mark = texture_for(graphics, Sprite::WolfCalled);
    SDL_SetTextureColorModFloat(mark, light.red, light.green, light.blue);
    SDL_FRect note{rect.x + rect.w * .27F, rect.y - rect.h * .34F, rect.w * .46F, rect.h * .46F};
    SDL_RenderTexture(renderer, mark, nullptr, &note);
    SDL_SetTextureColorModFloat(mark, 1, 1, 1);
    SDL_SetRenderDrawColorFloat(renderer, light.red * .72F, light.green * .77F, light.blue * .56F, 1);
    SDL_FRect bar{note.x, note.y + note.h, note.w * static_cast<float>(wolf.timer_c) / wolf_call_ticks, 1};
    SDL_RenderFillRect(renderer, &bar);
}
