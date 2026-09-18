#include "floating_render.hpp"
#include "floating_items.hpp"
#include "water.hpp"
#include "../items/air_bladder.hpp"
#include "../entities/river_raft.hpp"

void draw_personal_float(tr::Renderer* renderer, const GameGraphics& graphics,
    const Game& game, const Entity& actor, ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!personal_flotation(actor) || actor.toss.ticks>0 ||
        !river_water(game.stage.at_or_border(actor.cell).kind) || ridden_river_raft(game,actor)) return;
    SDL_FRect rect=tile_rect(actor.cell,camera,zoom);
    rect.x-=rect.w*.08F;rect.y+=rect.h*.46F;
    rect.w*=1.16F;rect.h*=.54F;
    const LightColor light=lit_sprite_color(lighting,actor.cell);
    tr::Texture* texture=texture_for(graphics,Sprite::ItemFloat);
    tr::texture_color(texture,light.red,light.green,light.blue);
    tr::draw_texture(renderer,texture,nullptr,&rect);
    tr::texture_color(texture,1,1,1);
}

void draw_item_float(tr::Renderer* renderer, const GameGraphics& graphics,
    const Entity& cargo, ViewCamera camera, float zoom, const LightingCache& lighting) {
    if (!floating_item(cargo)) return;
    SDL_FRect rect = tile_rect(cargo.cell, camera, zoom);
    rect.x += rect.w * .12F;
    rect.y += rect.h * .23F;
    rect.w *= .76F; rect.h *= .76F;
    const LightColor light = lit_sprite_color(lighting, cargo.cell);
    tr::Texture* texture = texture_for(graphics, Sprite::ItemFloat);
    tr::texture_color(texture, light.red, light.green, light.blue);
    tr::draw_texture(renderer, texture, nullptr, &rect);
    tr::texture_color(texture, 1, 1, 1);
}
