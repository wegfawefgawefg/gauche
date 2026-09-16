#include "floating_render.hpp"
#include "floating_items.hpp"
#include "water.hpp"
#include "../items/air_bladder.hpp"
#include "../entities/river_raft.hpp"

void draw_personal_float(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Entity& actor, ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!personal_flotation(actor) || actor.toss.ticks>0 ||
        !river_water(game.stage.at_or_border(actor.cell).kind) || ridden_river_raft(game,actor)) return;
    SDL_FRect rect=tile_rect(actor.cell,camera,zoom);
    rect.x-=rect.w*.08F;rect.y+=rect.h*.46F;
    rect.w*=1.16F;rect.h*=.54F;
    const LightColor light=lit_sprite_color(lighting,actor.cell);
    SDL_Texture* texture=texture_for(graphics,Sprite::ItemFloat);
    SDL_SetTextureColorModFloat(texture,light.red,light.green,light.blue);
    SDL_RenderTexture(renderer,texture,nullptr,&rect);
    SDL_SetTextureColorModFloat(texture,1,1,1);
}

void draw_item_float(SDL_Renderer* renderer, const GameGraphics& graphics,
    const Entity& cargo, ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!floating_item(cargo)) return;
    SDL_FRect rect = tile_rect(cargo.cell, camera, zoom);
    rect.x += rect.w * .12F;
    rect.y += rect.h * .23F;
    rect.w *= .76F; rect.h *= .76F;
    const LightColor light = lit_sprite_color(lighting, cargo.cell);
    SDL_Texture* texture = texture_for(graphics, Sprite::ItemFloat);
    SDL_SetTextureColorModFloat(texture, light.red, light.green, light.blue);
    SDL_RenderTexture(renderer, texture, nullptr, &rect);
    SDL_SetTextureColorModFloat(texture, 1, 1, 1);
}
