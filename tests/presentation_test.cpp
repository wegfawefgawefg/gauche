#include "../src/entities/behavior.hpp"
#include "../src/particles/system.hpp"
#include "../src/particles/templates.hpp"
#include "../src/view.hpp"

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
    spawn_campfire_smoke(cosmetics, {2, 2}, 4);
    bool animated = false;
    for (const SpriteParticle& particle : cosmetics.sprites)
        animated |= particle.motion == ParticleMotion::Animated;
    if (!check(animated, "campfire lost animated smoke")) return 1;
    std::puts("presentation rules passed");
    return 0;
}
