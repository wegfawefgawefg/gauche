#include "../src/entities/behavior.hpp"
#include "../src/particles/system.hpp"
#include "../src/particles/templates.hpp"
#include "../src/view.hpp"

#include <array>
#include <cstdio>

namespace {

bool has_sprite(const Cosmetics& cosmetics, Sprite id, ParticleLayer layer) {
    for (const SpriteParticle& particle : cosmetics.sprites)
        if (particle.sprite == id && particle.layer == layer) return true;
    return false;
}

bool check(bool okay, const char* message) {
    if (!okay) std::fprintf(stderr, "FAIL: %s\n", message);
    return okay;
}

} // namespace

int main() {
    const SDL_FRect centered = tile_rect({20, 12}, {20, 12}, 2.0F);
    if (!check(centered.x == 320.0F && centered.y == 180.0F &&
               centered.w == 16.0F && centered.h == 16.0F,
               "2x camera no longer matches Rust's half-size viewport")) return 1;
    Game game;
    game.stage.width = game.stage.height = 8;
    game.stage.tiles.resize(64);
    game.started = true;
    game.run.online[0] = true;
    game.players[0] = spawn_entity(game, EntityKind::Player, {2, 2});
    const Handle zombie = spawn_entity(game, EntityKind::Zombie, {4, 2});
    const Handle chicken = spawn_entity(game, EntityKind::Chicken, {5, 5});
    Cosmetics cosmetics;
    game.tick = 1;
    update_cosmetics(cosmetics, game, {2, 2});

    // Movement should leave distinct local prints without changing the game hash.
    game.tick = 2;
    move_entity(game, game.players[0].slot, {3, 2});
    move_entity(game, zombie.slot, {4, 3});
    const std::uint64_t before_prints = game_hash(game);
    update_cosmetics(cosmetics, game, {3, 2});
    if (!check(has_sprite(cosmetics, Sprite::PlayerFootprint, ParticleLayer::Ground) &&
               has_sprite(cosmetics, Sprite::ZombieFootprint, ParticleLayer::Ground),
               "movement did not leave both footprint types") ||
        !check(cosmetics.camera.x > 2.0F && cosmetics.camera.x < 3.0F,
               "camera snapped to the next tile") ||
        !check(game_hash(game) == before_prints, "footprints changed gameplay state"))
        return 1;

    Entity* player = get_entity(game, game.players[0]);
    player->facing = {0, -1};
    player->use_flash = 6;
    game.tick = 3;
    update_cosmetics(cosmetics, game, {3, 2});
    if (!check(cosmetics.poses[static_cast<std::size_t>(game.players[0].slot)].angle == 180.0F,
               "upward attack did not flip the attacker")) return 1;

    game.tick = 4;
    damage_entity(game, zombie.slot, 5, {3, 2});
    update_cosmetics(cosmetics, game, {3, 2});
    if (!check(has_sprite(cosmetics, Sprite::BloodSmall, ParticleLayer::Foreground) &&
               cosmetics.poses[static_cast<std::size_t>(zombie.slot)].shake > 0.0F,
               "hit lost blood or shake")) return 1;

    // The sweep removes enemies immediately, so the previous pose supplies the corpse.
    game.tick = 5;
    damage_entity(game, zombie.slot, 1000, {3, 2});
    remove_entity(game, zombie);
    update_cosmetics(cosmetics, game, {3, 2});
    if (!check(has_sprite(cosmetics, Sprite::ZombieDead, ParticleLayer::Ground),
               "dead zombie left no corpse")) return 1;

    const Entity* bird = get_entity(game, chicken);
    const SoundId call = bird->sprite == Sprite::Chick ? SoundId::Chick :
                         (bird->sprite == Sprite::Hen ? SoundId::Hen : SoundId::Rooster);
    const std::uint64_t gameplay_rng = game.rng;
    bool called = false;
    for (std::uint64_t tick = 6; tick < 100000 && !called; ++tick) {
        game.tick = tick;
        game.sound_count = 0;
        maybe_growl(game, chicken.slot, call);
        called = game.sound_count > 0 && game.sounds[0].sound == call;
    }
    if (!check(called && game.rng == gameplay_rng,
               "chicken call was absent or consumed gameplay RNG")) return 1;

    const SoundEvent blast{SoundId::Explosion, {3, 2}, game.tick, 0, true};
    spawn_sound_effect(cosmetics, blast, 12);
    bool arcs = false;
    for (const SpriteParticle& particle : cosmetics.sprites)
        arcs |= particle.motion == ParticleMotion::Arc;
    if (!check(!cosmetics.rings.empty() && !cosmetics.tile_shakes.empty() && arcs,
               "explosion lost its shockwave, tile shake, or arc debris")) return 1;
    game.sounds[0] = blast;
    game.sound_count = 1;
    const std::uint64_t before_flash = game_hash(game);
    update_cosmetics(cosmetics, game, {3, 2});
    if (!check(!cosmetics.flashes.empty() && game_hash(game) == before_flash,
               "explosion flash changed gameplay state")) return 1;
    for (int index = 0; index < 18; ++index) step_particles(cosmetics);
    if (!check(cosmetics.flashes.empty(), "explosion flash did not expire")) return 1;
    game.sound_count = 0;
    spawn_campfire_smoke(cosmetics, {2, 2}, 4);
    bool animated = false;
    for (const SpriteParticle& particle : cosmetics.sprites)
        animated |= particle.motion == ParticleMotion::Animated;
    if (!check(animated, "campfire lost animated smoke")) return 1;

    // The three cloud variants drift at Rust's slow rate on the weather plane.
    Cosmetics sky;
    std::array<bool, 3> cloud_variants{};
    for (std::uint64_t seed = 0; seed < 10000; ++seed) {
        const std::size_t before = sky.sprites.size();
        spawn_weather_cloud(sky, {20, 12}, seed, 2.0F);
        if (sky.sprites.size() == before) continue;
        const SpriteParticle& cloud = sky.sprites.back();
        const int variant = static_cast<int>(cloud.sprite) - static_cast<int>(Sprite::Cloud1);
        if (!check(cloud.layer == ParticleLayer::Weather && cloud.depth_height == 50 &&
                   cloud.vx >= 0.005F && cloud.vx <= 0.015F &&
                   cloud.width >= 4.0F && cloud.width <= 16.0F &&
                   cloud.span > 1000 && variant >= 0 && variant < 3,
                   "cloud lost its slow parallax recipe")) return 1;
        cloud_variants[static_cast<std::size_t>(variant)] = true;
        if (cloud_variants[0] && cloud_variants[1] && cloud_variants[2]) break;
    }
    if (!check(cloud_variants[0] && cloud_variants[1] && cloud_variants[2],
               "weather never selected all three cloud sprites")) return 1;

    Game scratch_game;
    scratch_game.stage.width = scratch_game.stage.height = 8;
    scratch_game.stage.tiles.resize(64);
    scratch_game.started = true;
    const Handle scratch_zombie = spawn_entity(scratch_game, EntityKind::Zombie, {3, 3});
    const Handle scratch_chicken = spawn_entity(scratch_game, EntityKind::Chicken, {4, 3});
    get_entity(scratch_game, scratch_zombie)->move_wait = 100;
    get_entity(scratch_game, scratch_chicken)->move_wait = 100;
    get_entity(scratch_game, scratch_chicken)->health = 30;
    Cosmetics scratch_cosmetics;
    scratch_game.tick = 1;
    update_cosmetics(scratch_cosmetics, scratch_game, {3, 3});
    step_game(scratch_game, {});
    update_cosmetics(scratch_cosmetics, scratch_game, {3, 3});
    if (!check(get_entity(scratch_game, scratch_zombie)->sprite == Sprite::Zombie &&
               has_sprite(scratch_cosmetics, Sprite::ZombieScratch1,
                          ParticleLayer::Foreground),
               "zombie scratch replaced the body instead of appearing beside it")) return 1;
    std::puts("presentation rules passed");
    return 0;
}
