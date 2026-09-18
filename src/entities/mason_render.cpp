#include "mason_render.hpp"
#include "ice_mason.hpp"

void draw_mason_block(tr::Renderer* renderer, const GameGraphics& graphics,
    const Entity& mason, SDL_FRect rect, LightColor light) {
    if (mason.counter_a == 0 || mason.health <= 0) return;
    // CARGO: The block stays visible even during an interrupted job or a defensive jab.
    const float forward = mason.label_a == MasonBuild ? .23F : .1F;
    SDL_FRect held{rect.x + rect.w * (.22F + static_cast<float>(mason.facing.x) * forward),
        rect.y + rect.h * (.57F + static_cast<float>(mason.facing.y) * forward * .35F), rect.w * .56F, rect.h * .42F};
    tr::Texture* texture = texture_for(graphics, Sprite::IceBlock);
    tr::texture_color(texture, light.red, light.green, light.blue);
    tr::draw_texture(renderer, texture, nullptr, &held);
    tr::texture_color(texture, 1, 1, 1);
}
