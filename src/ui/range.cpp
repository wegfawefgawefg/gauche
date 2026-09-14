#include "presentation.hpp"
#include "../view.hpp"

#include <algorithm>
#include <cmath>

ItemRange item_range(ItemKind kind) {
    switch (kind) {
    case ItemKind::Wall: return {1, 2};
    case ItemKind::Fist: case ItemKind::Stick: case ItemKind::Pickaxe:
    case ItemKind::Buckler: case ItemKind::BearTrap: case ItemKind::Mine: return {1, 1};
    case ItemKind::Bomb: case ItemKind::SleepMeds: return {0, 3};
    case ItemKind::Pistol: return {1, 9};
    case ItemKind::Shotgun: return {1, 5};
    case ItemKind::SMG: return {1, 8};
    case ItemKind::Musket: case ItemKind::Bow: case ItemKind::RocketLauncher:
        return {1, 14};
    default: return {};
    }
}

namespace {

bool in_range(Cell origin, Cell cell, ItemRange range) {
    const int steps = distance(origin, cell);
    return steps >= range.minimum && steps <= range.maximum;
}

void border(SDL_Renderer* renderer, Cell cell, ViewCamera camera, float zoom,
            Cell origin, ItemRange range) {
    const SDL_FRect rect = tile_rect(cell, camera, zoom);
    if (!in_range(origin, cell + Cell{0, -1}, range))
        SDL_RenderLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
    if (!in_range(origin, cell + Cell{0, 1}, range))
        SDL_RenderLine(renderer, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
    if (!in_range(origin, cell + Cell{-1, 0}, range))
        SDL_RenderLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
    if (!in_range(origin, cell + Cell{1, 0}, range))
        SDL_RenderLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
}

} // namespace

void draw_item_range_base(SDL_Renderer* renderer, const Entity& player,
                          ViewCamera camera, float zoom) {
    const ItemRange range = item_range(player.inventory.held()->kind);
    if (range.maximum == 0) return;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 40);
    for (int y = -range.maximum; y <= range.maximum; ++y) {
        for (int x = -range.maximum; x <= range.maximum; ++x) {
            const Cell cell = player.cell + Cell{x, y};
            if (!in_range(player.cell, cell, range)) continue;
            const SDL_FRect rect = tile_rect(cell, camera, zoom);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void draw_item_range_top(SDL_Renderer* renderer, const Entity& player, ViewCamera camera,
                         float zoom, const PointerState& pointer,
                         const GameGraphics& graphics) {
    const Item& held = *player.inventory.held();
    const ItemRange range = item_range(held.kind);
    if (range.maximum > 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 48);
        for (int y = -range.maximum; y <= range.maximum; ++y)
            for (int x = -range.maximum; x <= range.maximum; ++x) {
                const Cell cell = player.cell + Cell{x, y};
                if (in_range(player.cell, cell, range))
                    border(renderer, cell, camera, zoom, player.cell, range);
            }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    if (!pointer.inside || held.kind == ItemKind::None) return;
    const bool valid = range.maximum == 0 || in_range(player.cell, pointer.cell, range);
    SDL_FRect target = tile_rect(pointer.cell, camera, zoom);
    SDL_Texture* texture = texture_for(graphics, item_sprite(held.kind));
    const float side = target.w * 0.5F;
    target.x += (target.w - side) * 0.5F;
    target.y += (target.h - side) * 0.5F;
    target.w = target.h = side;
    SDL_SetTextureColorMod(texture, valid ? 255 : 70, valid ? 255 : 70,
                           valid ? 255 : 70);
    SDL_SetTextureAlphaMod(texture, 190);
    SDL_RenderTexture(renderer, texture, nullptr, &target);
    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, 255);
}
