#include "../props/scarecrow.hpp"
#include "presentation.hpp"
#include "../projectiles/projectile.hpp"
#include "../projectiles/net.hpp"
#include "../projectiles/root_drill.hpp"
#include "../item_pattern.hpp"
#include "../item_attribute.hpp"
#include "../items/materials.hpp"
#include "../items/mixtures.hpp"
#include "../entities/attacks.hpp"
#include "../entities/hearing.hpp"
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
    const Cell facing = player.label_b < 0 ? player.point_b : player.facing;
    const ItemPattern pattern = item_pattern(held);
    if (pattern.effect == PatternEffect::None || held.flight.slot >= 0) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (held.kind == ItemKind::Scarecrow) {
        const Cell ward = player.cell + facing;
        for (int y = -pattern.blast_radius; y <= pattern.blast_radius; ++y)
            for (int x = -pattern.blast_radius; x <= pattern.blast_radius; ++x) {
                const Cell cell = ward + Cell{x, y};
                if (scarecrow_covers(game, ward, cell, pattern.blast_radius))
                    mark(renderer, cell, camera, zoom, pattern.effect);
            }
    } else if (forest_mixture(held.kind) != nullptr) {
        const Cell center = bomb_landing(game, player.cell, facing, pattern.maximum);
        for (Cell cell : mixture_cells(game, held, center))
            mark(renderer, cell, camera, zoom, pattern.effect);
    } else if (held.kind == ItemKind::HandBell || held.kind == ItemKind::Firecracker) {
        const Cell center = held.kind == ItemKind::HandBell ? player.cell :
            bomb_landing(game, player.cell, facing, pattern.maximum);
        for (Cell cell : audible_cells(game, center, pattern.blast_radius))
            mark(renderer, cell, camera, zoom, pattern.effect);
    } else if (held.kind == ItemKind::ThrowingNet) {
        const Cell side{-facing.y, facing.x};
        int lanes = (1 << (pattern.half_width * 2 + 1)) - 1;
        for (int reach = 1; reach <= pattern.maximum; ++reach) {
            const Cell center = player.cell + Cell{facing.x * reach, facing.y * reach};
            lanes = net_open_lanes(game, center, side, pattern.half_width, lanes);
            if (lanes == 0) break;
            bool caught = false;
            for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
                if ((lanes & (1 << (lane + pattern.half_width))) == 0) continue;
                const Cell cell = center + Cell{side.x * lane, side.y * lane};
                mark(renderer, cell, camera, zoom, pattern.effect);
                const int target = entity_at(game, cell, true);
                caught |= target >= 0 && net_target(game.entities[static_cast<std::size_t>(target)]);
            }
            if (caught) break;
        }
    } else if (pattern.cross_blast) {
        const Cell center = player.cell + facing;
        mark(renderer, center, camera, zoom, pattern.effect);
        for (Cell direction : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
            Cell cell = center;
            for (int reach = 1; reach <= pattern.blast_radius; ++reach) {
                cell = cell + direction;
                if (projectile_blocked(game, cell)) break;
                mark(renderer, cell, camera, zoom, pattern.effect);
                if (entity_at(game, cell, true) >= 0) break;
            }
        }
    } else if (pattern.cone) {
        const Cell side{-facing.y, facing.x};
        for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach)
            for (int lane = -pattern_half_width(pattern, reach); lane <= pattern_half_width(pattern, reach); ++lane) {
                const Cell cell = player.cell + Cell{facing.x * reach + side.x * lane, facing.y * reach + side.y * lane};
                if (clear_sight(game, player.cell, cell, false)) mark(renderer, cell, camera, zoom, pattern.effect);
            }
    } else if (pattern.ray) {
        const Cell sideways{-facing.y, facing.x};
        for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        Cell cell = player.cell + Cell{sideways.x * lane, sideways.y * lane};
        for (int step = 1; step <= pattern.maximum; ++step) {
            cell = cell + facing;
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr) break;
            const bool pierced = pattern.piercing ||
                (pattern.effect == PatternEffect::Damage && has_artifact(player, ArtifactKind::AllPiercing));
            const bool obstacle = held.kind == ItemKind::RootDrill ? root_drill_blocked(game, cell, held) : projectile_blocked(game, cell);
            const bool impact = obstacle ||
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
    } else if (item_is_melee(held.kind) || pattern.half_width > 0) {
        const Cell sideways{-facing.y, facing.x};
        for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane)
            for (int reach = pattern.minimum; reach <= pattern.maximum; ++reach) {
                const Cell cell = player.cell +
                    Cell{facing.x * reach + sideways.x * lane,
                         facing.y * reach + sideways.y * lane};
                const Tile* tile = game.stage.at(cell);
                if (tile == nullptr) break;
                mark(renderer, cell, camera, zoom, pattern.effect);
                if (!walkable(*tile) || (!pattern.piercing && entity_at(game, cell, true) >= 0)) break;
            }
    } else {
        const Cell aim = pointer.left && pointer.inside ?
            pointer.cell - player.cell : facing;
        const RegionalItem* spec = forest_material_item(held.kind);
        const Cell target = held.kind == ItemKind::Bomb ||
            (spec != nullptr && spec->action == ItemAction::Throw) ?
            bomb_landing(game, player.cell, facing, pattern.maximum) :
            aimed_item_target(player, aim, pattern);
        if (pattern.blast_radius > 0) {
            for (int reach = 1; reach < distance(player.cell, target); ++reach)
                mark(renderer, player.cell +
                     Cell{facing.x * reach, facing.y * reach},
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
