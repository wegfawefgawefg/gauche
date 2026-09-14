#include "presentation.hpp"
#include "../projectiles/projectile.hpp"
#include "../item_pattern.hpp"
#include "../item_attribute.hpp"
#include "../view.hpp"

#include <algorithm>
#include <cstdlib>

ItemRange item_range(ItemKind kind) {
    const ItemPattern pattern = item_pattern(kind);
    return {pattern.minimum, pattern.maximum};
}

namespace {

void mark(SDL_Renderer* renderer, Cell cell, ViewCamera camera, float zoom,
          PatternEffect effect, bool travel = false) {
    const SDL_FRect rect = tile_rect(cell, camera, zoom);
    const std::uint8_t red = effect == PatternEffect::Damage ? 225 :
                             (effect == PatternEffect::Heal ? 91 : 223);
    const std::uint8_t green = effect == PatternEffect::Damage ? 71 :
                               (effect == PatternEffect::Heal ? 219 : 225);
    const std::uint8_t blue = effect == PatternEffect::Damage ? 57 :
                              (effect == PatternEffect::Heal ? 110 : 210);
    SDL_SetRenderDrawColor(renderer, red, green, blue, travel ? 30 : 49);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, red, green, blue, travel ? 90 : 155);
    if (travel) {
        const float side = rect.w * 0.28F;
        SDL_RenderLine(renderer, rect.x, rect.y, rect.x + side, rect.y);
        SDL_RenderLine(renderer, rect.x + rect.w - side, rect.y + rect.h,
                       rect.x + rect.w, rect.y + rect.h);
    } else SDL_RenderRect(renderer, &rect);
}

void blast_marks(SDL_Renderer* renderer, Cell center, int radius,
                 ViewCamera camera, float zoom, PatternEffect effect) {
    for (int dy = -radius; dy <= radius; ++dy)
        for (int dx = -radius; dx <= radius; ++dx)
            if (std::abs(dx) + std::abs(dy) <= radius)
                mark(renderer, center + Cell{dx, dy}, camera, zoom, effect);
}

} // namespace

void draw_item_range_top(SDL_Renderer* renderer, const GameGraphics& graphics,
                         const Game& game, const Entity& player, ViewCamera camera,
                         float zoom, const PointerState& pointer) {
    const Item& held = *player.inventory.held();
    const ItemPattern pattern = item_pattern(held);
    if (pattern.effect == PatternEffect::None) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (pattern.ray) {
        const Cell sideways{-player.facing.y, player.facing.x};
        for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        Cell cell = player.cell + Cell{sideways.x * lane, sideways.y * lane};
        for (int step = 1; step <= pattern.maximum; ++step) {
            cell = cell + player.facing;
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr) break;
            const bool pierced = pattern.piercing ||
                has_artifact(player, ArtifactKind::AllPiercing);
            const bool impact = (held.kind == ItemKind::Bow ? projectile_blocked(game, cell) : !walkable(*tile)) ||
                (entity_at(game, cell, true) >= 0 &&
                 (!pierced || held.kind == ItemKind::RocketLauncher)) ||
                step == pattern.maximum;
            if (pattern.blast_radius > 0) {
                mark(renderer, cell, camera, zoom, pattern.effect, !impact);
                if (impact) blast_marks(renderer, cell, pattern.blast_radius,
                                        camera, zoom, pattern.effect);
            } else mark(renderer, cell, camera, zoom, pattern.effect);
            if (impact) break;
        }
        }
    } else if (pattern.minimum == 0 && pattern.maximum == 0) {
        mark(renderer, player.cell, camera, zoom, pattern.effect);
    } else if (item_is_melee(held.kind)) {
        const Cell sideways{-player.facing.y, player.facing.x};
        for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane)
            for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach) {
                const Cell cell = player.cell +
                    Cell{player.facing.x * reach + sideways.x * lane,
                         player.facing.y * reach + sideways.y * lane};
                const Tile* tile = game.stage.at(cell);
                if (tile == nullptr) break;
                mark(renderer, cell, camera, zoom, pattern.effect);
                if (!walkable(*tile) || (!pattern.piercing && entity_at(game, cell, true) >= 0)) break;
            }
    } else {
        const Cell aim = pointer.left && pointer.inside ?
            pointer.cell - player.cell : player.facing;
        const Cell target = held.kind == ItemKind::Bomb ?
            bomb_landing(game, player.cell, player.facing, pattern.maximum) :
            aimed_item_target(player, aim, pattern);
        if (pattern.blast_radius > 0) {
            for (int reach = 1; reach < distance(player.cell, target); ++reach)
                mark(renderer, player.cell +
                     Cell{player.facing.x * reach, player.facing.y * reach},
                     camera, zoom, pattern.effect, true);
            blast_marks(renderer, target, pattern.blast_radius,
                        camera, zoom, pattern.effect);
        } else mark(renderer, target, camera, zoom, pattern.effect);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    if (!pointer.inside || held.kind == ItemKind::None) return;
    const SDL_FRect target = tile_rect(pointer.cell, camera, zoom);
    const float side = target.w * 0.45F;
    SDL_FRect icon{target.x + (target.w - side) * 0.5F,
                   target.y + (target.h - side) * 0.5F, side, side};
    SDL_Texture* texture = texture_for(graphics, item_sprite(held.kind));
    SDL_SetTextureAlphaMod(texture, 150);
    SDL_RenderTexture(renderer, texture, nullptr, &icon);
    SDL_SetTextureAlphaMod(texture, 255);
}
