#include "render.hpp"

#include <algorithm>
#include <cstdio>

namespace {

constexpr float tile_pixels = 32.0F;

SDL_FRect tile_rect(Cell cell, Cell camera) {
    return {320.0F + static_cast<float>(cell.x - camera.x) * tile_pixels,
            160.0F + static_cast<float>(cell.y - camera.y) * tile_pixels,
            tile_pixels, tile_pixels};
}

void sprite(SDL_Renderer* renderer, const Graphics& graphics, Sprite id, SDL_FRect rect) {
    SDL_RenderTexture(renderer, texture_for(graphics, id), nullptr, &rect);
}

Sprite tile_sprite(const Tile& tile, std::uint64_t tick) {
    switch (tile.kind) {
    case TileKind::Grass: return Sprite::Grass;
    case TileKind::Wall: return Sprite::Wall;
    case TileKind::Ruin: return Sprite::Ruin;
    case TileKind::Water:
        return ((tick / 60 + tile.water_phase) % 2 == 0) ? Sprite::Water3 : Sprite::Water4;
    case TileKind::Rail: return Sprite::Rail;
    default: return Sprite::Grass;
    }
}

void draw_world(SDL_Renderer* renderer, const Graphics& graphics, const Game& game, Cell camera) {
    for (int y = camera.y - 6; y <= camera.y + 6; ++y) {
        for (int x = camera.x - 11; x <= camera.x + 11; ++x) {
            const Cell cell{x, y};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || tile->kind == TileKind::Empty) continue;
            SDL_FRect rect = tile_rect(cell, camera);
            sprite(renderer, graphics, tile_sprite(*tile, game.tick), rect);
            if (tile->kind == TileKind::Wall && tile->hp < 100) {
                SDL_SetRenderDrawColor(renderer, 30, 15, 15, 115);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    for (const Entity& entity : game.entities) {
        if (entity.kind == EntityKind::None || entity.kind == EntityKind::RailLayer) continue;
        SDL_FRect rect = tile_rect(entity.cell, camera);
        if (rect.x < -tile_pixels || rect.x > 640.0F || rect.y < -tile_pixels || rect.y > 360.0F)
            continue;
        if (entity.kind == EntityKind::GroundItem) {
            rect.x += 8.0F; rect.y += 8.0F; rect.w = rect.h = 16.0F;
        }
        sprite(renderer, graphics, entity.sprite, rect);
        const Item* held = entity.inventory.held();
        if (held->kind != ItemKind::None && entity.kind != EntityKind::GroundItem) {
            const float forward = entity.use_flash > 0 ? 16.0F : 9.0F;
            SDL_FRect held_rect{rect.x + 8.0F + static_cast<float>(entity.facing.x) * forward,
                                rect.y + 8.0F + static_cast<float>(entity.facing.y) * forward,
                                16.0F, 16.0F};
            sprite(renderer, graphics, item_sprite(held->kind), held_rect);
            if (held->kind == ItemKind::Buckler) {
                SDL_SetRenderDrawColor(renderer, 168, 185, 192, 230);
                SDL_RenderRect(renderer, &held_rect);
            }
        }
        if (entity.health > 0 && entity.health < entity.max_health && entity.max_health < 1000000) {
            SDL_FRect bar{rect.x + 2.0F, rect.y - 4.0F,
                          28.0F * static_cast<float>(entity.health) /
                          static_cast<float>(entity.max_health), 2.0F};
            SDL_SetRenderDrawColor(renderer, 198, 65, 59, 255);
            SDL_RenderFillRect(renderer, &bar);
        }
    }
}

void draw_hud(SDL_Renderer* renderer, const Graphics& graphics, const Entity& player) {
    SDL_FRect panel{0.0F, 290.0F, 640.0F, 70.0F};
    SDL_SetRenderDrawColor(renderer, 12, 15, 17, 245);
    SDL_RenderFillRect(renderer, &panel);
    for (int index = 0; index < quick_slots; ++index) {
        const float x = 18.0F + static_cast<float>(index) * 66.0F;
        SDL_FRect slot{x, 300.0F, 56.0F, 52.0F};
        SDL_SetRenderDrawColor(renderer, index == player.inventory.selected ? 207 : 72,
                              index == player.inventory.selected ? 192 : 78,
                              index == player.inventory.selected ? 123 : 79, 255);
        SDL_RenderRect(renderer, &slot);
        const Item& item = player.inventory.slots[static_cast<std::size_t>(index)];
        if (item.kind != ItemKind::None) {
            SDL_FRect icon{x + 12.0F, 305.0F, 30.0F, 30.0F};
            sprite(renderer, graphics, item_sprite(item.kind), icon);
            char count[24];
            std::snprintf(count, sizeof(count), "%d", item.count);
            SDL_RenderDebugText(renderer, x + 34.0F, 339.0F, count);
        }
        char key[2]{static_cast<char>('1' + index), '\0'};
        SDL_RenderDebugText(renderer, x + 3.0F, 339.0F, key);
    }
    char status[96];
    const Item& held = *player.inventory.held();
    std::snprintf(status, sizeof(status), "HP %d/%d  %s", player.health, player.max_health,
                  item_name(held.kind));
    SDL_RenderDebugText(renderer, 425.0F, 308.0F, status);
    if (held.loaded > 0 || held.spare > 0) {
        std::snprintf(status, sizeof(status), "AMMO %d/%d", held.loaded, held.spare);
        SDL_RenderDebugText(renderer, 425.0F, 325.0F, status);
    } else if (held.kind == ItemKind::Buckler) {
        std::snprintf(status, sizeof(status), "SHIELD %d", held.durability);
        SDL_RenderDebugText(renderer, 425.0F, 325.0F, status);
    }
}

} // namespace

void render_game(SDL_Renderer* renderer, const Graphics& graphics, const Game& game) {
    const Entity* player = get_entity(game, game.players[0]);
    const Cell camera = player == nullptr ? Cell{32, 32} : player->cell;
    draw_world(renderer, graphics, game, camera);
    if (player != nullptr) draw_hud(renderer, graphics, *player);
    if (game.game_over) {
        SDL_SetRenderDrawColor(renderer, 8, 8, 9, 190);
        SDL_FRect shade{0.0F, 0.0F, 640.0F, 360.0F};
        SDL_RenderFillRect(renderer, &shade);
        SDL_RenderDebugText(renderer, 260.0F, 166.0F, "GAME OVER");
        SDL_RenderDebugText(renderer, 230.0F, 187.0F, "PRESS ENTER TO RESTART");
    }
}
