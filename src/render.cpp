#include "combat/parry.hpp"
#include "render.hpp"
#include "entities/bell_diver.hpp"
#include "entities/flight_render.hpp"
#include "entities/plant_render.hpp"
#include "entities/wolf_render.hpp"
#include "items/fire_render.hpp"
#include "surfaces/render.hpp"
#include "entities/foraging.hpp"
#include "scenery/overhead.hpp"
#include "projectiles/render.hpp"
#include "debug/panels.hpp"
#include "entities/intent_render.hpp"
#include "particles/system.hpp"
#include "particles/motion.hpp"
#include "lighting/field.hpp"
#include "lighting/render.hpp"
#include "lighting/canopy.hpp"
#include "ui/presentation.hpp"
#include "props/render.hpp"
#include "ui/scale.hpp"
#include "view.hpp"
#include "world/wall_render.hpp"
#include "world/ice_render.hpp"
#include "world/encounter.hpp"

#include <algorithm>
#include <cstdio>
#include <cmath>

namespace {

Sprite tile_sprite(const Tile& tile, std::uint64_t tick, Cell cell, int world) {
    if (tile.material == TileMaterial::Tree)
        return tile.kind == TileKind::Wall ? Sprite::ForestTree : Sprite::TreeStump;
    if (tile.material == TileMaterial::Timber)
        return tile.kind == TileKind::Wall ? Sprite::ForestTimber : Sprite::TimberBroken;
    if (world == 2) {
        const Sprite native = ice_tile_sprite(tile, cell, tick);
        if (native != Sprite::Count) return native;
    }
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
    case TileKind::Snow: return Sprite::Snow;
    case TileKind::IceHole: return Sprite::IceHole;
    case TileKind::Grass: return Sprite::Grass;
    case TileKind::Wall: return Sprite::Wall;
    case TileKind::Ruin: return Sprite::Ruin;
    case TileKind::ShallowWater:
        return (tick / 45) % 2 == 0 ? Sprite::ShallowWaterA : Sprite::ShallowWaterB;
    case TileKind::Spring:
        return (tick / 12) % 2 == 0 ? Sprite::SpringA : Sprite::SpringB;
    case TileKind::Water:
        return ((tick / 60 + tile.water_phase) % 2 == 0) ? Sprite::Water3 : Sprite::Water4;
    case TileKind::Rail: return Sprite::Rail;
    default: return Sprite::Grass;
    }
}

void draw_tile_damage(SDL_Renderer* renderer, const Tile& tile, Cell cell,
                      SDL_FRect rect, const LightingCache& lighting) {
    if (tile.kind != TileKind::Wall || tile.hp >= tile.max_hp || tile.max_hp == 0) return;
    const float remaining = static_cast<float>(tile.hp) / static_cast<float>(tile.max_hp);
    const LightColor light = lit_sprite_color(lighting, cell);
    // FRACTURES: The same branching cuts work on every wall material.
    SDL_SetRenderDrawColorFloat(renderer, light.red * 0.08F, light.green * 0.07F,
                               light.blue * 0.06F, 1.0F);
    const float flip = ((cell.x + cell.y) % 2 == 0) ? 1.0F : -1.0F;
    const float cx = rect.x + rect.w * 0.5F;
    const float cy = rect.y + rect.h * 0.48F;
    SDL_RenderLine(renderer, cx, cy, cx + flip * rect.w * 0.16F, cy - rect.h * 0.26F);
    SDL_RenderLine(renderer, cx, cy, cx - flip * rect.w * 0.20F, cy + rect.h * 0.22F);
    if (remaining < 0.65F) {
        SDL_RenderLine(renderer, cx, cy, cx + flip * rect.w * 0.33F, cy + rect.h * 0.16F);
        SDL_RenderLine(renderer, cx - flip * rect.w * 0.1F, cy + rect.h * 0.11F,
                       cx - flip * rect.w * 0.35F, cy + rect.h * 0.06F);
    }
    if (remaining < 0.3F)
        SDL_RenderLine(renderer, cx, cy, cx - flip * rect.w * 0.38F, cy - rect.h * 0.35F);
    // CONDITION: Dark-room bars inherit the material's light instead of glowing through walls.
    SDL_FRect bar{rect.x + rect.w * 0.12F, rect.y + rect.h * 0.87F,
                  rect.w * 0.76F, std::max(1.0F, rect.h * 0.045F)};
    SDL_RenderFillRect(renderer, &bar);
    bar.w *= remaining;
    SDL_SetRenderDrawColorFloat(renderer, light.red * 0.88F, light.green * 0.68F,
                               light.blue * 0.40F, 1.0F);
    SDL_RenderFillRect(renderer, &bar);
}

void draw_tiles(SDL_Renderer* renderer, const GameGraphics& graphics,
                const Game& game, ViewCamera camera, float zoom,
                const Cosmetics* cosmetics, const LightingCache& lighting) {
    const float pixels = tile_pixels(zoom);
    const int columns = static_cast<int>(std::ceil(320.0F / pixels)) + 2;
    const int rows = static_cast<int>(std::ceil(200.0F / pixels)) + 2;
    for (int y = static_cast<int>(std::floor(camera.y)) - rows;
         y <= static_cast<int>(std::ceil(camera.y)) + rows; ++y) {
        for (int x = static_cast<int>(std::floor(camera.x)) - columns;
             x <= static_cast<int>(std::ceil(camera.x)) + columns; ++x) {
            const Cell cell{x, y};
            const Tile& tile = game.stage.at_or_border(cell);
            if (tile.kind == TileKind::Empty && game.run.phase == RunPhase::Arena) continue;
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
            const Sprite id = tile_sprite(tile, game.tick, cell, world);
            SDL_Texture* texture = texture_for(graphics, id);
            const LightColor tint = world == 1 && tile.kind != TileKind::Lava ?
                LightColor{225.0F / 255.0F, 133.0F / 255.0F, 105.0F / 255.0F} :
                LightColor{1.0F, 1.0F, 1.0F};
            if (lighting.active) draw_lit_tile(renderer, texture, rect, cell, lighting, tint);
            else {
                SDL_SetTextureColorModFloat(texture, tint.red, tint.green, tint.blue);
                SDL_RenderTexture(renderer, texture, nullptr, &rect);
                SDL_SetTextureColorModFloat(texture, 1.0F, 1.0F, 1.0F);
            }
            if (world >= 0 && tile.kind == TileKind::Wall)
                draw_wall_contour(renderer, game.stage, cell, rect, lighting,
                    world == 2 ? LightColor{.78F, .9F, 1.25F} : tint);
            draw_tile_damage(renderer, tile, cell, rect, lighting);
        }
    }
}

void draw_entities(SDL_Renderer* renderer, const GameGraphics& graphics,
                   const Game& game, ViewCamera camera, float zoom,
                   const Cosmetics* cosmetics, const LightingCache& lighting, int layer) {
    const float pixels = tile_pixels(zoom);
    // LAYERS: Fixtures sit on the ground, pickups above them, then actors with held items.
    for (std::size_t slot = 0; slot < game.entities.size(); ++slot) {
        const Entity& entity = game.entities[slot];
        if (entity.kind == EntityKind::None || entity.kind == EntityKind::RailLayer ||
            ((entity.kind == EntityKind::Door || entity.kind == EntityKind::EncounterGate) && entity.fixture_open)) continue;
        const int entity_layer = diver_submerged(entity) || entity.kind == EntityKind::Campfire || entity.kind == EntityKind::PocketDoor ||
            entity.kind == EntityKind::Trap || entity.kind == EntityKind::Exit ||
            entity.kind == EntityKind::Switch || entity.kind == EntityKind::Encounter ||
            entity.kind == EntityKind::WaveVent ? 0 :
            entity.kind == EntityKind::GroundItem || entity.kind == EntityKind::Key ||
            entity.kind == EntityKind::Coins ? 1 : 2;
        if (entity_layer != layer) continue;
        if (entity.kind == EntityKind::Projectile) {
            draw_projectile(renderer, graphics, entity, game, camera, zoom, lighting);
            continue;
        }
        SDL_FRect rect = tile_rect(entity.cell, camera, zoom);
        const EntityPose* pose = cosmetics == nullptr ? nullptr : &cosmetics->poses[slot];
        // TILE TRUTH: Body and held-item origins agree with collisions; only the camera is smoothed.
        if (rect.x < -pixels || rect.x > 640.0F || rect.y < -pixels || rect.y > 360.0F + (entity.kind == EntityKind::ZombieStack ? pixels * 3 : 0))
            continue;
        if (entity.kind == EntityKind::GroundItem || entity.kind == EntityKind::Key ||
            entity.kind == EntityKind::Coins) {
            rect.x += pixels * 0.25F; rect.y += pixels * 0.25F;
            rect.w = rect.h = pixels * 0.5F;
        }
        if (entity.sprite == Sprite::Chick) {
            rect.x += pixels * 0.25F; rect.y += pixels * 0.25F;
            rect.w = rect.h = pixels * 0.5F;
        }
        if (entity.kind == EntityKind::Train) {
            rect.x -= pixels * 0.5F; rect.y -= pixels * 0.5F;
            rect.w = rect.h = pixels * 2.0F;
        }
        if (pose != nullptr && pose->seen && pose->generation == entity.generation &&
            pose->shake > 0.0F) {
            const std::uint32_t jitter = static_cast<std::uint32_t>(
                game.tick * 1103515245ULL + slot * 12345ULL);
            rect.x += (static_cast<float>(jitter & 255U) / 127.5F - 1.0F) *
                      pose->shake * pixels;
            rect.y += (static_cast<float>((jitter >> 8) & 255U) / 127.5F - 1.0F) *
                      pose->shake * pixels;
        }
        SDL_Texture* texture = texture_for(graphics, entity.sprite);
        const LightColor self = entity.max_health > 0 && entity.health <= 0 ?
            LightColor{} : light_color(entity.self_light);
        const LightColor brightness = lit_sprite_color(lighting, entity.cell, self);
        SDL_SetTextureColorModFloat(texture, brightness.red,
                                    brightness.green, brightness.blue);
        const int bodies = entity.kind == EntityKind::ZombieStack ?
                           std::clamp(entity.counter_a, 1, 5) : 1;
        for (int body = 0; body < bodies; ++body) {
            SDL_FRect body_rect = rect;
            double angle = pose != nullptr && pose->seen ? pose->angle : 0.0;
            if (eats_meat(entity.kind) && entity.label_b == 1) {
                body_rect.h *= .88F; body_rect.y += pixels * .12F;
                angle += std::sin(static_cast<double>(game.tick % 60) * .65) * 7;
            }
            // TELLS: Keep the creature visible while its committed attack winds up.
            if (entity.kind == EntityKind::Bear && entity.label_a == 1) {
                body_rect.y -= pixels * .18F;
                body_rect.h += pixels * .18F;
                angle -= entity.facing.x < 0 ? -12.0 : 12.0;
            }
            if (entity.kind == EntityKind::Chicken && entity.label_a == 0 && entity.move_wait > 0) {
                const float scurry = std::sin(static_cast<float>(game.tick % 120) * (entity.timer_a > 0 ? 1.7F : .8F));
                body_rect.y -= pixels * .035F * std::abs(scurry);
                angle += static_cast<double>(scurry * (entity.timer_a > 0 ? 18 : 8));
            }
            if (entity.kind == EntityKind::LanternMoth) {
                const float wing = .84F + .16F * std::cos(static_cast<float>(game.tick % 60) * .7F);
                body_rect.x += body_rect.w * (1.0F - wing) * .5F;
                body_rect.w *= wing;
            }
            if (body > 0) {
                const float sway = std::sin(static_cast<float>(game.tick % 6000) * .05F +
                                           static_cast<float>(body)) * .08F;
                body_rect.x += pixels * sway * static_cast<float>(body);
                body_rect.y -= pixels * .58F * static_cast<float>(body);
                angle += static_cast<double>(sway * 80);
                if (entity.label_a != 0) angle += body % 2 == 0 ? 35 : -35;
            }
            // FALL: New survivors tumble out from the old stack's cell during their stun.
            if (entity.kind == EntityKind::Zombie && entity.label_b == 1 && entity.timer_b > 0) {
                const float remaining = static_cast<float>(entity.timer_b) / 24.0F;
                body_rect.x += static_cast<float>(entity.point_a.x - entity.cell.x) * pixels * remaining;
                body_rect.y += (static_cast<float>(entity.point_a.y - entity.cell.y) - .6F) * pixels * remaining;
                angle += static_cast<double>(remaining * 270);
            }
            apply_flight_pose(entity, game.tick, body_rect, angle);
            if (entity.kind == EntityKind::Trap && entity.ground_item.kind == ItemKind::SpringTrap)
                angle = std::atan2(static_cast<double>(entity.facing.y), static_cast<double>(entity.facing.x)) * 180.0 / 3.141592653589793;
            const bool worm = entity.kind == EntityKind::BurrowWorm;
            if (worm) angle = std::atan2(static_cast<double>(entity.facing.y),
                static_cast<double>(entity.facing.x)) * 180.0 / 3.141592653589793;

            SDL_RenderTextureRotated(renderer, texture, nullptr, &body_rect, angle,
                nullptr, !worm && pose != nullptr && pose->horizontal_flip ?
                         SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        }
        SDL_SetTextureAlphaMod(texture, 255);
        SDL_SetTextureColorModFloat(texture, 1.0F, 1.0F, 1.0F);
        if (entity.kind == EntityKind::GroundItem)
            draw_item_flame(renderer, graphics, entity.ground_item, rect, {1, 0}, game.tick);
        // PAIRS: Both thresholds carry the same small letter beneath passing actors.
        if (entity.kind == EntityKind::PocketDoor && entity.fixture_open) {
            const int pair = std::min(static_cast<int>(slot), entity.entity_a.slot);
            const char label[]{static_cast<char>('A' + std::max(0, pair) % 26), '\0'};
            SDL_SetRenderDrawColorFloat(renderer, brightness.red * .8F, brightness.green * .9F, brightness.blue, 1);
            SDL_RenderDebugText(renderer, rect.x + rect.w * .25F, rect.y + rect.h * .25F, label);
        }
        if (entity.kind == EntityKind::RootTurret) draw_root_head(renderer, entity, rect, brightness);
        draw_wolf_call(renderer, graphics, entity, rect, brightness);
        if (entity.vitals.rooted > 0 && entity.health > 0) {
            const bool netted = entity.vitals.root_kind == RootKind::Net;
            SDL_Texture* rope = texture_for(graphics, netted ? Sprite::NetCaught : Sprite::SnareTight);
            SDL_SetTextureColorModFloat(rope, brightness.red, brightness.green, brightness.blue);
            SDL_FRect feet = netted ? rect : SDL_FRect{rect.x, rect.y + rect.h * .55F, rect.w, rect.h * .45F};
            SDL_RenderTexture(renderer, rope, nullptr, &feet);
            SDL_SetTextureColorModFloat(rope, 1, 1, 1);
        }
        const Item* held = entity.inventory.held();
        if (held->kind != ItemKind::None && held->flight.slot < 0 && entity.kind != EntityKind::GroundItem) {
            const bool winding = entity.kind == EntityKind::Player && entity.label_b < 0;
            const Cell held_facing = winding ? entity.point_b : entity.facing;
            const float forward = winding ? -pixels * .1F : entity.use_flash > 0 ? pixels * 0.5F : pixels * 0.28F;
            SDL_FRect held_rect{rect.x + pixels * 0.25F +
                                static_cast<float>(held_facing.x) * forward,
                                rect.y + pixels * 0.25F +
                                static_cast<float>(held_facing.y) * forward,
                                pixels * 0.5F, pixels * 0.5F};
            const double angle = std::atan2(static_cast<double>(held_facing.y),
                                            static_cast<double>(held_facing.x)) *
                                 180.0 / 3.141592653589793;
            const Sprite held_sprite = parry_active(entity) ? Sprite::PanReady : held->kind == ItemKind::Bow && entity.kind == EntityKind::Player &&
                entity.counter_a > 0 ? Sprite::BowDrawn : item_sprite(*held);
            SDL_Texture* held_texture = texture_for(graphics, held_sprite);
            SDL_SetTextureColorModFloat(held_texture, brightness.red,
                                        brightness.green, brightness.blue);
            SDL_RenderTextureRotated(renderer, held_texture, nullptr, &held_rect,
                                     angle, nullptr, held_facing.x < 0 ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
            SDL_SetTextureColorModFloat(held_texture, 1.0F, 1.0F, 1.0F);
            draw_item_flame(renderer, graphics, *held, held_rect, held_facing, game.tick);
            if (held->kind == ItemKind::Buckler ||
                (held->kind == ItemKind::ShieldLantern && entity.block_ticks > 0)) {
                SDL_SetRenderDrawColor(renderer, 168, 185, 192, 230);
                SDL_RenderRect(renderer, &held_rect);
            }
        }
        if (entity.kind != EntityKind::Player && entity.health > 0 &&
            entity.health < entity.max_health && entity.max_health < 1000000) {
            SDL_FRect bar{rect.x + 2.0F, rect.y - 4.0F,
                          (rect.w - 4.0F) * static_cast<float>(entity.health) /
                          static_cast<float>(entity.max_health), 2.0F};
            SDL_SetRenderDrawColor(renderer, 198, 65, 59, 255);
            SDL_RenderFillRect(renderer, &bar);
        }
    }
}

void draw_run_status(SDL_Renderer* renderer, const Game& game, float zoom) {
    const HudScale scale{renderer};
    SDL_SetRenderDrawColor(renderer, 235, 230, 214, 255);
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
    SDL_RenderDebugText(renderer, 640.0F / ui_scale - 103.0F, 12.0F, zoom_label);
}

} // namespace

void render_game(SDL_Renderer* renderer, const GameGraphics& graphics,
                 const Game& game, int local_owner, float zoom,
                 const Cosmetics* cosmetics, const PointerState& pointer,
                 bool show_hud, bool compact_details) {
    const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(local_owner)]);
    const ViewCamera camera = cosmetics != nullptr ?
        camera_for(*cosmetics, game, local_owner) :
        ViewCamera{player == nullptr ? Cell{32, 32} : player->cell};
    LightingCache local_lighting;
    LightingCache& lighting = cosmetics != nullptr ? cosmetics->lighting : local_lighting;
    build_lighting(lighting, game, camera, zoom,
                   cosmetics != nullptr ? std::span<const LightFlash>{cosmetics->flashes} :
                                          std::span<const LightFlash>{});
    draw_tiles(renderer, graphics, game, camera, zoom, cosmetics, lighting);
    draw_surfaces(renderer, game, camera, zoom, lighting, false);
    draw_props(renderer, graphics, game.stage, camera, zoom, lighting);
    if (cosmetics != nullptr)
        draw_debris(renderer, graphics, cosmetics->debris, camera, zoom, lighting);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Ground,
                       camera, zoom, &lighting, &game.stage);
    draw_owl_landing(renderer, graphics, game, camera, zoom, lighting);
    if (debug_panels().world_enemies) draw_enemy_intents(renderer, game, camera, zoom, lighting);
    draw_entities(renderer, graphics, game, camera, zoom, cosmetics, lighting, 0);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Flames, camera, zoom, &lighting, &game.stage);
    draw_entities(renderer, graphics, game, camera, zoom, cosmetics, lighting, 1);
    draw_entities(renderer, graphics, game, camera, zoom, cosmetics, lighting, 2);
    draw_plant_lash(renderer, game, camera, zoom, lighting);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Foreground,
                       camera, zoom, &lighting, &game.stage);
    if (player != nullptr && debug_panels().world_items)
        draw_item_range_top(renderer, graphics, game, *player, camera, zoom, pointer);
    draw_surfaces(renderer, game, camera, zoom, lighting, true);
    draw_overhead(renderer, graphics, game, cosmetics, camera, zoom, lighting);
    draw_canopy_shafts(renderer, game, camera, zoom);
    if (player != nullptr && show_hud)
        draw_hud(renderer, graphics, game, *player, pointer, compact_details);
    draw_run_status(renderer, game, zoom);
    if (player != nullptr) draw_encounter_status(renderer, game, *player);
}

void render_title_backdrop(SDL_Renderer* renderer, const GameGraphics& graphics,
                           const Game& scene) {
    const ViewCamera camera = scene.run.spawn + Cell{2, 0};
    LightingCache lighting;
    build_lighting(lighting, scene, camera, 2.0F);
    draw_tiles(renderer, graphics, scene, camera, 2.0F, nullptr, lighting);
    draw_props(renderer, graphics, scene.stage, camera, 2.0F, lighting);
    for (int layer = 0; layer < 3; ++layer)
        draw_entities(renderer, graphics, scene, camera, 2.0F, nullptr, lighting, layer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 3, 7, 7, 172);
    const SDL_FRect shade{0.0F, 0.0F, 640.0F, 360.0F};
    SDL_RenderFillRect(renderer, &shade);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
