#include "world/reactor_render.hpp"
#include "scene_entities.hpp"
#include "props/rail_render.hpp"
#include "entities/freight_render.hpp"
#include "entities/choir_render.hpp"
#include "entities/mold_render.hpp"
#include "items/machine_fitting_render.hpp"
#include "entities/pump_render.hpp"
#include "entities/counterweight_render.hpp"
#include "entities/ash_sleeper.hpp"
#include "entities/furnace_moth.hpp"
#include "items/pocket_drill.hpp"
#include "entities/pressure_rat.hpp"
#include "entities/crane_render.hpp"
#include "combat/toss.hpp"
#include "entities/yeti.hpp"
#include "items/rivet_gun.hpp"
#include "entities/breaker_render.hpp"
#include "items/ice_anchor_render.hpp"
#include "scenery/ice_render.hpp"
#include "combat/parry.hpp"
#include "render.hpp"
#include "items/effigy_mask_render.hpp"
#include "entities/gate_render.hpp"
#include "world/floating_render.hpp"
#include "entities/bell_diver.hpp"
#include "entities/leech_render.hpp"
#include "entities/eel_render.hpp"
#include "entities/mason_render.hpp"
#include "entities/knight_render.hpp"
#include "entities/drummer_render.hpp"
#include "entities/warden_render.hpp"
#include "entities/shard_render.hpp"
#include "entities/boiler_render.hpp"
#include "entities/flight_render.hpp"
#include "entities/plant_render.hpp"
#include "entities/wolf_render.hpp"
#include "items/fire_render.hpp"
#include "items/quarry_render.hpp"
#include "surfaces/render.hpp"
#include "entities/foraging.hpp"
#include "scenery/overhead.hpp"
#include "scenery/shadows.hpp"
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
#include "world/industrial_render.hpp"
#include "world/encounter.hpp"

#include <algorithm>
#include <cstdio>
#include <cmath>

namespace {

Sprite tile_sprite(const Tile& tile, std::uint64_t tick, Cell cell, Biome biome, bool arena) {
    if (tile.surface.still_ticks>0) tick=0;
    if (tile.kind == TileKind::Ice && tile.freeze_ticks > 0)
        return tile.freeze_ticks <= 120 ? Sprite::ThawingWater : Sprite::FrozenWater;
    if (tile.material == TileMaterial::Ice && tile.kind == TileKind::Wall) return Sprite::IceWall;
    if (tile.material == TileMaterial::Tree)
        return tile.kind == TileKind::Wall ? Sprite::ForestTree : Sprite::TreeStump;
    if (tile.material == TileMaterial::Timber)
        return tile.kind == TileKind::Wall ? Sprite::ForestTimber : Sprite::TimberBroken;
    if (!arena && biome==Biome::Ice) {
        const Sprite native = ice_tile_sprite(tile, cell, tick);
        if (native != Sprite::Count) return native;
    }
    if (!arena && biome==Biome::Industrial) {
        const Sprite native=industrial_tile_sprite(tile,cell,tick);
        if (native!=Sprite::Count) return native;
    }
    if (!arena) {
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
    case TileKind::Bridge:
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
            const bool arena=game.run.phase==RunPhase::Arena;
            const Biome biome=floor_biome(game.run.floor);
            if (draw_freight_track(renderer,graphics,game.stage,cell,rect,lighting)) {
                draw_tile_damage(renderer,tile,cell,rect,lighting);continue;
            }
            const Sprite id = tile_sprite(tile, game.tick, cell, biome, arena);
            SDL_Texture* texture = texture_for(graphics, id);
            const LightColor tint{1.0F, 1.0F, 1.0F};
            if (lighting.active) draw_lit_tile(renderer, texture, rect, cell, lighting, tint);
            else {
                SDL_SetTextureColorModFloat(texture, tint.red, tint.green, tint.blue);
                SDL_RenderTexture(renderer, texture, nullptr, &rect);
                SDL_SetTextureColorModFloat(texture, 1.0F, 1.0F, 1.0F);
            }
            if (!arena && tile.kind == TileKind::Wall)
                draw_wall_contour(renderer, game.stage, cell, rect, lighting,
                    biome==Biome::Ice ? LightColor{.78F, .9F, 1.25F} :
                    biome==Biome::Industrial ? LightColor{1.05F,.88F,1.08F} : tint);
            draw_tile_damage(renderer, tile, cell, rect, lighting);
        }
    }
}


void draw_run_status(SDL_Renderer* renderer, const Game& game, float zoom) {
    const HudScale scale{renderer};
    SDL_SetRenderDrawColor(renderer, 235, 230, 214, 255);
    if (game.run.phase != RunPhase::Arena) {
        char floor[64];
        std::snprintf(floor, sizeof(floor), "%s %d/4   %s", biome_name(floor_biome(game.run.floor)),
                      biome_stage(game.run.floor),
                      game.run.has_key ? "DOOR OPEN" :
                      (game.run.layout==FloorLayout::LastShift ? "FIND SHUTDOWN" : game.run.objective == ObjectiveKind::Key ? "FIND KEY" : "FIND SWITCH"));
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
    draw_reactor_hazards(renderer,graphics,game,camera,zoom);
    draw_contact_shadows(renderer,game,cosmetics,camera,zoom);
    if (cosmetics) draw_ice_scenery(renderer,graphics,game,*cosmetics,camera,zoom,lighting);
    draw_props(renderer, graphics, game.stage, camera, zoom, lighting,game.tick);
    for (const Entity& actor : game.entities)
        if (actor.kind == EntityKind::LensWarden) draw_warden_charge(renderer, graphics, game, actor, camera, zoom, lighting);
    if (cosmetics != nullptr)
        draw_debris(renderer, graphics, cosmetics->debris, camera, zoom, lighting);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Ground,
                       camera, zoom, &lighting, &game.stage);
    draw_owl_landing(renderer, graphics, game, camera, zoom, lighting);
    draw_shard_links(renderer,game,camera,zoom,lighting);
    if (debug_panels().world_enemies) draw_enemy_intents(renderer, game, camera, zoom, lighting);
    draw_crane_parts(renderer,graphics,game,camera,zoom,lighting,true);
    draw_counterweights(renderer,graphics,game,camera,zoom,lighting,true);
    draw_entities(renderer, graphics, game, camera, zoom, cosmetics, lighting, 0);
    if (cosmetics != nullptr)
        draw_particles(renderer, graphics, *cosmetics, ParticleLayer::Flames, camera, zoom, &lighting, &game.stage);
    draw_entities(renderer, graphics, game, camera, zoom, cosmetics, lighting, 1);
    draw_entities(renderer, graphics, game, camera, zoom, cosmetics, lighting, 2);
    draw_crane_parts(renderer,graphics,game,camera,zoom,lighting,false);
    draw_counterweights(renderer,graphics,game,camera,zoom,lighting,false);
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
    draw_reactor_status(renderer,game);
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
