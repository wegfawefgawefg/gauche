#include "system.hpp"

#include <algorithm>
#include <cmath>

namespace {
// MATERIALS: Sprite indices need not be adjacent as new biomes add their own scraps.
constexpr std::array<Sprite, static_cast<std::size_t>(DebrisKind::Count)> sprites{
    Sprite::DebrisOakLeaf, Sprite::DebrisBirchLeaf, Sprite::DebrisPineNeedle,
    Sprite::DebrisTwig, Sprite::DebrisBark, Sprite::DebrisWoodChip, Sprite::DebrisRoot,
    Sprite::DebrisFernLeaf, Sprite::DebrisGrassBlade, Sprite::DebrisMushroomCap,
    Sprite::DebrisMushroomStem, Sprite::DebrisSpore, Sprite::DebrisAcorn,
    Sprite::DebrisSeedHusk, Sprite::DebrisFeather, Sprite::DebrisBoneChip,
    Sprite::DebrisPottery, Sprite::DebrisCloth, Sprite::DebrisBrassCase,
    Sprite::DebrisStoneChip, Sprite::DebrisStraw, Sprite::DebrisIceChip, Sprite::DebrisSnowClump,
};
}

void draw_debris(SDL_Renderer* renderer, const GameGraphics& graphics,
                  const LooseDebris& debris, ViewCamera camera, float zoom,
                  const LightingCache& lighting) {
    const float pixels = tile_pixels(zoom);
    for (const LoosePiece& p : debris.pieces) {
        const float x = view_center_x + (p.x - camera.x) * pixels;
        const float y = view_center_y + (p.y - camera.y) * pixels;
        if (x < -pixels || y < -pixels || x > 640 + pixels || y > 360 + pixels) continue;
        const float size = pixels * (.35F + static_cast<float>(std::min<int>(p.count, 6)) * .025F);
        const SDL_FRect rect{x - size * .5F, y - size * .5F, size, size};
        const Sprite sprite = sprites[static_cast<std::size_t>(p.kind)];
        SDL_Texture* texture = texture_for(graphics, sprite);
        const LightColor light = light_at_cell(lighting,
            {static_cast<int>(std::floor(p.x)), static_cast<int>(std::floor(p.y))});
        SDL_SetTextureColorModFloat(texture, light.red * .78F, light.green * .78F, light.blue * .78F);
        SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, p.angle, nullptr, SDL_FLIP_NONE);
        SDL_SetTextureColorModFloat(texture, 1, 1, 1);
    }
}
