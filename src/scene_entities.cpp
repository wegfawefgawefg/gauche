#include "props/light_tower_render.hpp"
#include "props/tall_tree_render.hpp"
#include "entities/boiler_feed_render.hpp"
#include "entities/cutter_render.hpp"
#include "props/ice_pillar_render.hpp"
#include "items/cooking_render.hpp"
#include "scene_entities.hpp"
#include "entities/bear_family.hpp"
#include "scenery/roof_render.hpp"
#include "props/streetlamp_render.hpp"
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
void draw_suspended_parts(SDL_Renderer* renderer,const GameGraphics& graphics,const Game& game,
                          const Entity& entity,ViewCamera camera,float zoom,const LightingCache& lighting) {
    if (entity.kind==EntityKind::MagnetCrane)
        draw_crane_parts(renderer,graphics,game,camera,zoom,lighting,false,&entity);
    if (entity.kind==EntityKind::Counterweight)
        draw_counterweights(renderer,graphics,game,camera,zoom,lighting,false,&entity);
}
}

void draw_entities(SDL_Renderer* renderer, const GameGraphics& graphics,
                   const Game& game, ViewCamera camera, float zoom,
                   const Cosmetics* cosmetics, const LightingCache& lighting, ScenePass pass,const Entity* viewer) {
    const float pixels = tile_pixels(zoom);
    // Ground effects precede the shared ground-anchor order for raised bodies.
    for (const BodyDraw& entry:body_draw_order(game,camera,zoom,pass)) {
        if (entry.kind==BodyKind::RoofRow) {
            const auto& roof=game.stage.roofs[entry.slot];
            draw_roof_row(renderer,graphics,roof,entry.cell.y-roof.start.y,viewer,camera,zoom,lighting);
            continue;
        }
        if (entry.kind==BodyKind::Prop) {
            if (game.stage.at(entry.cell)->prop.kind==PropKind::LightTower)
                draw_light_tower(renderer,graphics,game,entry.cell,camera,zoom,lighting);
            else if (game.stage.at(entry.cell)->prop.kind==PropKind::TallTree)
                draw_tall_tree(renderer,graphics,game,entry.cell,camera,zoom,lighting);
            else if (game.stage.at(entry.cell)->prop.kind==PropKind::IcePillar)
                draw_ice_pillar(renderer,graphics,game,entry.cell,camera,zoom,lighting);
            else draw_streetlamp(renderer,graphics,game,entry.cell,camera,zoom,lighting);
            continue;
        }
        const std::size_t slot=entry.slot;
        const Entity& entity = game.entities[slot];
        if (entity.max_health>0 && entity.health<=0 && game.stage.at_or_border(entity.cell).kind==TileKind::Chasm) continue;
        if (entity.kind == EntityKind::None || entity.kind == EntityKind::RailLayer ||
            (entity.kind == EntityKind::Door && entity.fixture_open)) continue;
        if (entity.kind == EntityKind::EncounterGate) {
            draw_gate(renderer,graphics,entity,camera,zoom,lighting);
            continue;
        }
        if (entity.kind==EntityKind::IceAnchor) draw_anchor_tether(renderer,game,entity,camera,zoom,lighting);
        if (entity.kind == EntityKind::Projectile) {
            draw_projectile(renderer, graphics, entity, game, camera, zoom, lighting);
            continue;
        }
        if (entity.kind == EntityKind::WhiteoutDrummer) draw_drummer_warning(renderer, game, entity, camera, zoom, lighting);
        SDL_FRect rect = tile_rect(entity.cell, camera, zoom);
        const EntityPose* pose = cosmetics == nullptr ? nullptr : &cosmetics->poses[slot];
        // TILE TRUTH: Body and held-item origins agree with collisions; only the camera is smoothed.
        if (rect.x < -pixels || rect.x > 640.0F || rect.y < -pixels || rect.y > 360.0F + (entity.kind == EntityKind::ZombieStack ? pixels * 3 : 0)) {
            // A crane base can be off-screen while its extended head is visible.
            draw_suspended_parts(renderer,graphics,game,entity,camera,zoom,lighting);
            continue;
        }
        if (entity.kind==EntityKind::Bear) {
            const float size=bear_size(entity);
            rect.x-=pixels*(size-1)*.5F;rect.y-=pixels*(size-1);rect.w*=size;rect.h*=size;
        }
        if (entity.sprite==Sprite::ReactorCore) {
            rect.x-=pixels*.5F;rect.y-=pixels;rect.w*=2;rect.h*=2;
        }
        if (entity.kind == EntityKind::GroundItem)
            draw_item_float(renderer, graphics, entity, camera, zoom, lighting);
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
        rect.y-=actor_toss_height(entity)*pixels;
        SDL_Texture* texture = texture_for(graphics, entity.kind == EntityKind::GroundItem &&
            (entity.ground_item.kind==ItemKind::LunchTin || entity.ground_item.kind==ItemKind::GlowSlag || entity.ground_item.kind == ItemKind::SteamKettle || entity.ground_item.kind==ItemKind::SteamLance || entity.ground_item.kind == ItemKind::HeatSiphon) ? item_sprite(entity.ground_item) : entity.sprite);
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
            if (entity.kind==EntityKind::Sled) angle=entity.facing.x>0 ? 0 : entity.facing.x<0 ? 180 : entity.facing.y>0 ? 90 : -90;
            if (eats_meat(entity.kind) && entity.label_b == 1) {
                body_rect.h *= .88F; body_rect.y += pixels * .12F;
                angle += std::sin(static_cast<double>(game.tick % 60) * .65) * 7;
            }
            if (entity.kind==EntityKind::Yeti && entity.label_a==YetiGrab) {
                const float reach=1-static_cast<float>(entity.timer_a)/36;
                body_rect.h*=1+.18F*reach; body_rect.y-=pixels*.18F*reach;
                angle+=entity.facing.x<0 ? 15*reach : -15*reach;
            }
            if (entity.kind==EntityKind::EmergencyPump || entity.kind==EntityKind::SlagSnail || entity.kind==EntityKind::WalkingKiln || entity.kind==EntityKind::PressureRat || entity.kind==EntityKind::CableCrawler) {
                if (entity.facing.y!=0) angle=entity.facing.y<0 ? -90 : 90;
                if (entity.kind==EntityKind::PressureRat && entity.label_a==RatInflate) {
                    const float inset=.14F*static_cast<float>(entity.timer_a)/36;
                    body_rect.x+=pixels*inset;body_rect.y+=pixels*inset;
                    body_rect.w-=pixels*2*inset;body_rect.h-=pixels*2*inset;
                }
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
            if (entity.kind == EntityKind::LanternMoth || entity.kind==EntityKind::FurnaceMoth) {
                const bool warning=entity.kind==EntityKind::FurnaceMoth && entity.label_a==FurnaceWarn;
                const float wing = warning ? .60F : .84F + .16F * std::cos(static_cast<float>(game.tick % 60) * .7F);
                if (entity.kind==EntityKind::FurnaceMoth) {
                    body_rect.y+=body_rect.h*(1-wing)*.5F;body_rect.h*=wing;
                } else {
                    body_rect.x += body_rect.w * (1.0F - wing) * .5F;body_rect.w *= wing;
                }
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
            apply_freight_pose(entity,body_rect,angle);
            apply_choir_pose(entity,game.tick,body_rect,angle);
            apply_mold_pose(entity,body_rect,angle);
            apply_pump_pose(entity,body_rect);
            apply_flight_pose(entity, game.tick, body_rect, angle);
            if (entity.kind==EntityKind::AshSleeper) {
                if (entity.label_a==AshRising || entity.label_a==AshSwipe) angle+=entity.timer_a/3%2==0 ? -7 : 7;
                if (entity.label_a==AshSettling) {const float size=.4F+.6F*static_cast<float>(entity.timer_a)/30;body_rect.y+=body_rect.h*(1-size);body_rect.h*=size;}
            }
            apply_leech_pose(entity, game.tick, body_rect, angle);
            apply_eel_pose(entity, game.tick, body_rect, angle);
            if (entity.kind == EntityKind::Trap && entity.ground_item.kind == ItemKind::SpringTrap)
                angle = std::atan2(static_cast<double>(entity.facing.y), static_cast<double>(entity.facing.x)) * 180.0 / 3.141592653589793;
            const bool worm = entity.kind == EntityKind::BurrowWorm;
            if (worm) angle = std::atan2(static_cast<double>(entity.facing.y),
                static_cast<double>(entity.facing.x)) * 180.0 / 3.141592653589793;
            pose_canine(entity,body_rect,angle);

            SDL_RenderTextureRotated(renderer, texture, nullptr, &body_rect, angle,
                nullptr, ((entity.kind==EntityKind::Dog || (entity.kind==EntityKind::Wolf && entity.label_a!=0) || entity.kind==EntityKind::EmergencyPump || entity.kind==EntityKind::SlagSnail || entity.kind==EntityKind::WalkingKiln || entity.kind==EntityKind::PressureRat || entity.kind==EntityKind::CableCrawler) ? entity.facing.x<0 :
                    !worm && entity.kind != EntityKind::GlassEel && entity.kind != EntityKind::SteamLeech && pose != nullptr && pose->horizontal_flip) ?
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
        if (entity.kind == EntityKind::Strikebreaker) draw_breaker_shield(renderer,graphics,entity,rect,brightness);
        if (entity.kind == EntityKind::MirrorKnight) draw_knight_shield(renderer, graphics, entity, rect, brightness);
        if (entity.kind == EntityKind::IceMason) draw_mason_block(renderer, graphics, entity, rect, brightness);
        if (entity.kind == EntityKind::RootTurret) draw_root_head(renderer, entity, rect, brightness);
        draw_cutter_details(renderer,graphics,entity,rect,brightness);
        draw_freight_details(renderer,graphics,entity,rect,brightness);
        draw_mold_details(renderer,graphics,entity,rect,brightness);
        draw_machine_fitting(renderer,graphics,entity,rect,brightness);
        draw_pump_nozzle(renderer,entity,rect,brightness);
        draw_wolf_call(renderer, graphics, entity, rect, brightness);
        if (entity.kind == EntityKind::SteamLeech)
            draw_leech_tether(renderer, game, entity, camera, zoom, lighting);
        if (entity.vitals.rooted > 0 && entity.health > 0) {
            const bool netted = entity.vitals.root_kind == RootKind::Net;
            SDL_Texture* rope = texture_for(graphics, netted ? Sprite::NetCaught : Sprite::SnareTight);
            SDL_SetTextureColorModFloat(rope, brightness.red, brightness.green, brightness.blue);
            SDL_FRect feet = netted ? rect : SDL_FRect{rect.x, rect.y + rect.h * .55F, rect.w, rect.h * .45F};
            SDL_RenderTexture(renderer, rope, nullptr, &feet);
            SDL_SetTextureColorModFloat(rope, 1, 1, 1);
        }
        Item displayed_gun;
        const Item* held = entity.inventory.held();
        if (entity.kind==EntityKind::RivetGunner || entity.kind==EntityKind::ArcWelder) {
            displayed_gun=make_item(entity.kind==EntityKind::ArcWelder ? ItemKind::ArcTorch : ItemKind::RivetGun); held=&displayed_gun;
        }
        if (held->kind != ItemKind::None && (held->flight.slot < 0 || held->kind == ItemKind::HarpoonGun || held->kind == ItemKind::ChainHook) && entity.kind != EntityKind::GroundItem) {
            const bool winding = entity.kind == EntityKind::Player && entity.label_b < 0;
            const Cell held_facing = (winding || rivet_burst_active(entity)) ? entity.point_b : entity.facing;
            const float forward = pocket_drill_active(entity) ? pixels*(.38F+(game.tick%2==0 ? .015F : -.015F)) : winding ? -pixels * .1F : entity.use_flash > 0 ? pixels * 0.5F : pixels * 0.28F;
            SDL_FRect held_rect{rect.x + pixels * 0.25F +
                                static_cast<float>(held_facing.x) * forward,
                                rect.y + pixels * 0.25F +
                                static_cast<float>(held_facing.y) * forward,
                                pixels * 0.5F, pixels * 0.5F};
            if (held->kind==ItemKind::SteelToeCap) held_rect.y+=pixels*.2F;
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
        draw_effigy_mask(renderer,graphics,entity,rect,brightness);
        draw_boiler_details(renderer,graphics,entity,rect,brightness);
        draw_boiler_water(renderer,graphics,game,entity,rect,brightness);
        draw_suspended_parts(renderer,graphics,game,entity,camera,zoom,lighting);
        draw_brick_prepare(renderer, entity, rect, brightness);
        draw_cooking(renderer,graphics,entity,rect,brightness);
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
