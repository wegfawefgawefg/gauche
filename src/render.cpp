#include "render.hpp"
#include "particles/system.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cmath>

namespace {

float tile_pixels(float zoom) { return 16.0F * zoom; }

SDL_FRect tile_rect(Cell cell, Cell camera, float zoom) {
    const float pixels = tile_pixels(zoom);
    return {320.0F + static_cast<float>(cell.x - camera.x) * pixels,
            160.0F + static_cast<float>(cell.y - camera.y) * pixels,
            pixels, pixels};
}

void sprite(SDL_Renderer* renderer, const GameGraphics& graphics, Sprite id, SDL_FRect rect) {
    SDL_RenderTexture(renderer, texture_for(graphics, id), nullptr, &rect);
}

Sprite tile_sprite(const Tile& tile, std::uint64_t tick, Cell cell, int world) {
    if (world >= 0) {
        switch (tile.kind) {
        case TileKind::Empty: {
            const unsigned int variant = static_cast<unsigned int>(cell.x * 17 + cell.y * 31) % 3U;
            return variant == 0 ? Sprite::ForestFloorA :
                   (variant == 1 ? Sprite::ForestFloorB : Sprite::ForestFloorC);
        }
        case TileKind::Grass: return Sprite::ForestGrass;
        case TileKind::Wall: return Sprite::ForestWall;
        case TileKind::Ruin: return Sprite::ForestRuin;
        case TileKind::Lava: return Sprite::LavaTile;
        case TileKind::Ice: return Sprite::IceTile;
        default: break;
        }
    }
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

void draw_tiles(SDL_Renderer* renderer, const GameGraphics& graphics,
                const Game& game, Cell camera, float zoom, const Cosmetics* cosmetics) {
    const float pixels = tile_pixels(zoom);
    const int columns = static_cast<int>(std::ceil(320.0F / pixels)) + 2;
    const int rows = static_cast<int>(std::ceil(200.0F / pixels)) + 2;
    for (int y = std::max(0, camera.y - rows);
         y <= std::min(game.stage.height - 1, camera.y + rows); ++y) {
        for (int x = std::max(0, camera.x - columns);
             x <= std::min(game.stage.width - 1, camera.x + columns); ++x) {
            const Cell cell{x, y};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr ||
                (tile->kind == TileKind::Empty && game.run.phase == RunPhase::Arena)) continue;
            SDL_FRect rect = tile_rect(cell, camera, zoom);
            if (cosmetics != nullptr) {
                float strength = 0.0F;
                for (const TileShake& shake : cosmetics->tile_shakes)
                    if (shake.cell == cell) strength = std::max(strength, shake.strength);
                if (strength > 0.0F) {
                    const std::uint32_t jitter = static_cast<std::uint32_t>(
                        game.tick * std::uint64_t{747796405} +
                        static_cast<std::uint64_t>(x) * std::uint64_t{2891336453} +
                        static_cast<std::uint64_t>(y) * std::uint64_t{1181783497});
                    rect.x += (static_cast<float>(jitter & 255U) / 127.5F - 1.0F) *
                              strength * pixels;
                    rect.y += (static_cast<float>((jitter >> 8) & 255U) / 127.5F - 1.0F) *
                              strength * pixels;
                }
            }
            const int world = game.run.phase == RunPhase::Arena ? -1 :
                              (game.run.floor - 1) / 4;
            const Sprite id = tile_sprite(*tile, game.tick, cell, world);
            SDL_Texture* texture = texture_for(graphics, id);
            if (world == 1 && tile->kind != TileKind::Lava)
                SDL_SetTextureColorMod(texture, 225, 133, 105);
            if (world == 2 && tile->kind != TileKind::Ice)
                SDL_SetTextureColorMod(texture, 149, 201, 229);
            SDL_RenderTexture(renderer, texture, nullptr, &rect);
            SDL_SetTextureColorMod(texture, 255, 255, 255);
            if (tile->kind == TileKind::Wall && tile->hp < 100) {
                SDL_SetRenderDrawColor(renderer, 30, 15, 15, 115);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void draw_entities(SDL_Renderer* renderer, const GameGraphics& graphics,
                   const Game& game, Cell camera, Cell focus, float zoom,
                   const Cosmetics* cosmetics) {
    const float pixels = tile_pixels(zoom);
    for (std::size_t slot = 0; slot < game.entities.size(); ++slot) {
        const Entity& entity = game.entities[slot];
        if (entity.kind == EntityKind::None || entity.kind == EntityKind::RailLayer ||
            (entity.kind == EntityKind::Door && entity.fixture_open)) continue;
        SDL_FRect rect = tile_rect(entity.cell, camera, zoom);
        if (rect.x < -pixels || rect.x > 640.0F || rect.y < -pixels || rect.y > 360.0F)
            continue;
        if (entity.kind == EntityKind::GroundItem || entity.kind == EntityKind::Key) {
            rect.x += pixels * 0.25F; rect.y += pixels * 0.25F;
            rect.w = rect.h = pixels * 0.5F;
        }
        const EntityPose* pose = cosmetics == nullptr ? nullptr : &cosmetics->poses[slot];
        if (pose != nullptr && pose->seen && pose->generation == entity.generation &&
            pose->shake > 0.0F) {
            const std::uint32_t jitter = static_cast<std::uint32_t>(
                game.tick * 1103515245ULL + slot * 12345ULL);
            rect.x += (static_cast<float>(jitter & 255U) / 127.5F - 1.0F) *
                      pose->shake * pixels;
            rect.y += (static_cast<float>((jitter >> 8) & 255U) / 127.5F - 1.0F) *
                      pose->shake * pixels;
        }
        const float dx = static_cast<float>(entity.cell.x - focus.x);
        const float dy = static_cast<float>(entity.cell.y - focus.y);
        const float view_alpha = entity.kind == EntityKind::Player ? 1.0F :
            std::clamp(1.0F - std::sqrt(dx * dx + dy * dy) / 12.0F, 0.0F, 1.0F);
        SDL_Texture* texture = texture_for(graphics, entity.sprite);
        SDL_SetTextureAlphaMod(texture, static_cast<std::uint8_t>(view_alpha * 255.0F));
        SDL_RenderTextureRotated(renderer, texture, nullptr, &rect,
            pose != nullptr && pose->seen ? pose->angle : 0.0,
            nullptr, pose != nullptr && pose->horizontal_flip ?
                     SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        SDL_SetTextureAlphaMod(texture, 255);
        const Item* held = entity.inventory.held();
        if (held->kind != ItemKind::None && entity.kind != EntityKind::GroundItem) {
            const float forward = entity.use_flash > 0 ? pixels * 0.5F : pixels * 0.28F;
            SDL_FRect held_rect{rect.x + pixels * 0.25F +
                                static_cast<float>(entity.facing.x) * forward,
                                rect.y + pixels * 0.25F +
                                static_cast<float>(entity.facing.y) * forward,
                                pixels * 0.5F, pixels * 0.5F};
            const double angle = std::atan2(static_cast<double>(entity.facing.y),
                                            static_cast<double>(entity.facing.x)) *
                                 180.0 / 3.141592653589793;
            SDL_RenderTextureRotated(renderer, texture_for(graphics, item_sprite(held->kind)),
                                     nullptr, &held_rect, angle, nullptr, SDL_FLIP_NONE);
            if (held->kind == ItemKind::Buckler) {
                SDL_SetRenderDrawColor(renderer, 168, 185, 192, 230);
                SDL_RenderRect(renderer, &held_rect);
            }
        }
        if (entity.kind != EntityKind::Player && entity.health > 0 &&
            entity.health < entity.max_health && entity.max_health < 1000000) {
            SDL_FRect bar{rect.x + 2.0F, rect.y - 4.0F,
                          28.0F * static_cast<float>(entity.health) /
                          static_cast<float>(entity.max_health), 2.0F};
            SDL_SetRenderDrawColor(renderer, 198, 65, 59, 255);
            SDL_RenderFillRect(renderer, &bar);
        }
    }
}

bool clear_light_path(const Stage& stage, Cell from, Cell to) {
    int x = from.x;
    int y = from.y;
    const int dx = std::abs(to.x - from.x);
    const int dy = std::abs(to.y - from.y);
    const int sx = from.x < to.x ? 1 : -1;
    const int sy = from.y < to.y ? 1 : -1;
    int error = dx - dy;
    while (x != to.x || y != to.y) {
        const int twice = error * 2;
        if (twice > -dy) { error -= dy; x += sx; }
        if (twice < dx) { error += dx; y += sy; }
        if (x == to.x && y == to.y) break;
        const Tile* tile = stage.at({x, y});
        if (tile != nullptr && tile->kind == TileKind::Wall) return false;
    }
    return true;
}

float light_from(const Stage& stage, Cell source, Cell cell, float radius) {
    const float dx = static_cast<float>(source.x - cell.x);
    const float dy = static_cast<float>(source.y - cell.y);
    const float distance_to_light = std::sqrt(dx * dx + dy * dy);
    if (distance_to_light >= radius || !clear_light_path(stage, source, cell)) return 0.0F;
    return 1.0F - distance_to_light / radius;
}

void draw_lighting(SDL_Renderer* renderer, const Game& game, Cell camera,
                   int local_owner, float zoom) {
    if (game.run.phase == RunPhase::Arena) return;
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(local_owner)]);
    std::array<Cell, 24> fires{};
    int fire_count = 0;
    for (const Entity& entity : game.entities) {
        if ((entity.kind == EntityKind::Campfire || entity.kind == EntityKind::Ember) &&
            fire_count < static_cast<int>(fires.size()))
            fires[static_cast<std::size_t>(fire_count++)] = entity.cell;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const float pixels = tile_pixels(zoom);
    const int columns = static_cast<int>(std::ceil(320.0F / pixels)) + 2;
    const int rows = static_cast<int>(std::ceil(200.0F / pixels)) + 2;
    for (int y = std::max(0, camera.y - rows);
         y <= std::min(game.stage.height - 1, camera.y + rows); ++y) {
        for (int x = std::max(0, camera.x - columns);
             x <= std::min(game.stage.width - 1, camera.x + columns); ++x) {
            const Cell cell{x, y};
            if (!game.stage.in_bounds(cell)) continue;
            float light = 0.34F;
            if (player != nullptr) light = std::max(light,
                light_from(game.stage, player->cell, cell, 7.0F));
            for (int index = 0; index < game.run.roof_light_count; ++index)
                light = std::max(light, light_from(game.stage,
                    game.run.roof_lights[static_cast<std::size_t>(index)], cell, 5.0F));
            light = std::max(light, light_from(game.stage, game.run.exit, cell, 4.0F));
            for (int index = 0; index < fire_count; ++index)
                light = std::max(light, light_from(game.stage,
                    fires[static_cast<std::size_t>(index)], cell, 5.5F));
            if (const Tile* tile = game.stage.at(cell);
                tile != nullptr && tile->kind == TileKind::Lava)
                light = std::max(light, 0.78F);
            const auto darkness = static_cast<std::uint8_t>((1.0F - light) * 160.0F);
            SDL_SetRenderDrawColor(renderer, 3, 6, 8, darkness);
            SDL_FRect rect = tile_rect(cell, camera, zoom);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

const char* artifact_name(ArtifactKind kind) {
    switch (kind) {
    case ArtifactKind::AllPiercing: return "All Piercing";
    case ArtifactKind::Reflector: return "Reflector";
    case ArtifactKind::Hearth: return "Hearth";
    case ArtifactKind::FleetFeet: return "Fleet Feet";
    default: return "Artifact";
    }
}

void draw_interlude(SDL_Renderer* renderer, const GameGraphics& graphics,
                    const Game& game, int local_owner) {
    const bool pending = game.run.phase == RunPhase::Playing &&
                         game.run.pending_count[static_cast<std::size_t>(local_owner)] > 0;
    if (game.run.phase != RunPhase::Reward && game.run.phase != RunPhase::Shop &&
        game.run.phase != RunPhase::Won && !pending) return;
    SDL_SetRenderDrawColor(renderer, 8, 12, 13, 228);
    const SDL_FRect overlay{0.0F, 0.0F, 640.0F, 360.0F};
    SDL_RenderFillRect(renderer, &overlay);
    if (game.run.phase == RunPhase::Won) {
        SDL_RenderDebugText(renderer, 256.0F, 140.0F, "RUN CLEARED");
        return;
    }
    SDL_RenderDebugText(renderer, 35.0F, 32.0F,
                        pending ? "CHOOSE MISSED REWARD" :
                        (game.run.phase == RunPhase::Reward ?
                            "CHOOSE ONE REWARD" : "TRAVELING SHOP"));
    for (int index = 0; index < 3; ++index) {
        const float x = 35.0F + static_cast<float>(index) * 196.0F;
        SDL_FRect card{x, 70.0F, 178.0F, 165.0F};
        SDL_SetRenderDrawColor(renderer, 177, 159, 106, 255);
        SDL_RenderRect(renderer, &card);
        char key[8];
        std::snprintf(key, sizeof(key), "%d", index + 1);
        SDL_RenderDebugText(renderer, x + 10.0F, 81.0F, key);
        if (game.run.phase == RunPhase::Reward || pending) {
            const Reward reward = pending ?
                game.run.pending_offers[static_cast<std::size_t>(local_owner)][0]
                                       [static_cast<std::size_t>(index)] :
                game.run.offers[static_cast<std::size_t>(local_owner)]
                               [static_cast<std::size_t>(index)];
            const char* name = "";
            switch (reward.kind) {
            case RewardKind::Item:
                name = item_name(reward.item);
                sprite(renderer, graphics, item_sprite(reward.item),
                       {x + 69.0F, 111.0F, 40.0F, 40.0F});
                break;
            case RewardKind::Artifact: name = artifact_name(reward.artifact); break;
            case RewardKind::Health: name = "+20 Max HP"; break;
            case RewardKind::Speed: name = "Faster Steps"; break;
            }
            SDL_RenderDebugText(renderer, x + 10.0F, 165.0F, name);
        } else {
            const ItemKind kind = game.run.shop_stock[static_cast<std::size_t>(index)];
            if (kind != ItemKind::None)
                sprite(renderer, graphics, item_sprite(kind),
                       {x + 69.0F, 111.0F, 40.0F, 40.0F});
            SDL_RenderDebugText(renderer, x + 10.0F, 165.0F, item_name(kind));
            if (kind != ItemKind::None) {
                char cost[24];
                std::snprintf(cost, sizeof(cost), "%d coins", shop_price(kind));
                SDL_RenderDebugText(renderer, x + 10.0F, 189.0F, cost);
            }
        }
    }
    if (game.run.phase == RunPhase::Reward || pending) {
        SDL_RenderDebugText(renderer, 35.0F, 265.0F, "1-3 CHOOSE    Q DROP HELD ITEM IF PACK FULL");
    } else {
        char coins[32];
        std::snprintf(coins, sizeof(coins), "COINS %d",
                      game.run.coins[static_cast<std::size_t>(local_owner)]);
        SDL_RenderDebugText(renderer, 35.0F, 265.0F, coins);
        SDL_RenderDebugText(renderer, 35.0F, 289.0F, "ENTER TO CONTINUE");
    }
}

void draw_hud(SDL_Renderer* renderer, const GameGraphics& graphics, const Entity& player) {
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

void render_game(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Game& game, int local_owner, bool can_restart, float zoom,
                 const Cosmetics* cosmetics) {
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(local_owner)]);
    const Cell camera = player == nullptr ? Cell{32, 32} : player->cell;
    draw_tiles(renderer, graphics, game, camera, zoom, cosmetics);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Ground, camera, zoom);
    draw_entities(renderer, graphics, game, camera, camera, zoom, cosmetics);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Foreground, camera, zoom);
    draw_lighting(renderer, game, camera, local_owner, zoom);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Weather, camera, zoom);
    if (player != nullptr) draw_hud(renderer, graphics, *player);
    if (game.run.phase != RunPhase::Arena) {
        char floor[64];
        constexpr const char* worlds[]{"FOREST", "FIRE", "ICE"};
        const int world = std::clamp((game.run.floor - 1) / 4, 0, 2);
        std::snprintf(floor, sizeof(floor), "%s %d/4   %s", worlds[world],
                      (game.run.floor - 1) % 4 + 1,
                      game.run.has_key ? "DOOR OPEN" :
                      (game.run.objective == ObjectiveKind::Key ? "FIND KEY" : "FIND SWITCH"));
        SDL_RenderDebugText(renderer, 18.0F, 12.0F, floor);
    }
    char zoom_label[24];
    std::snprintf(zoom_label, sizeof(zoom_label), "ZOOM %.2fX", static_cast<double>(zoom));
    SDL_RenderDebugText(renderer, 537.0F, 12.0F, zoom_label);
    draw_interlude(renderer, graphics, game, local_owner);
    if (game.run.phase == RunPhase::Won)
        SDL_RenderDebugText(renderer, 230.0F, 190.0F,
                            can_restart ? "PRESS ENTER TO RESTART" : "WAIT FOR HOST");
    if (game.game_over) {
        SDL_SetRenderDrawColor(renderer, 8, 8, 9, 190);
        SDL_FRect shade{0.0F, 0.0F, 640.0F, 360.0F};
        SDL_RenderFillRect(renderer, &shade);
        SDL_RenderDebugText(renderer, 260.0F, 166.0F, "GAME OVER");
        SDL_RenderDebugText(renderer, 230.0F, 187.0F,
                            can_restart ? "PRESS ENTER TO RESTART" : "WAIT FOR HOST");
    }
}

void render_title_backdrop(SDL_Renderer* renderer, const GameGraphics& graphics,
                           const Game& scene) {
    const Cell camera = scene.run.spawn + Cell{2, 0};
    draw_tiles(renderer, graphics, scene, camera, 2.0F, nullptr);
    draw_entities(renderer, graphics, scene, camera, camera, 2.0F, nullptr);
    draw_lighting(renderer, scene, camera, 0, 2.0F);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 3, 7, 7, 172);
    const SDL_FRect shade{0.0F, 0.0F, 640.0F, 360.0F};
    SDL_RenderFillRect(renderer, &shade);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
