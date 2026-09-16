#include "chasm.hpp"
#include "../items/action.hpp"
#include "../items/sled.hpp"
#include "../projectiles/projectile.hpp"
#include "water.hpp"

bool grounded_projectile(const Entity& actor) {
    if (actor.counter_a>0) return false;
    switch (static_cast<ProjectileKind>(actor.label_a)) {
    case ProjectileKind::Bomb: case ProjectileKind::Firecracker: case ProjectileKind::PrismBomb:
    case ProjectileKind::QuarryCharge: case ProjectileKind::ThawCharge: case ProjectileKind::FoamCan:
    case ProjectileKind::Flare: case ProjectileKind::EchoPebble: return true;
    case ProjectileKind::Mixture: return actor.ground_item.kind==ItemKind::PitchBomb;
    default: return false; // Traveling shots and contact bursts pass over missing floor.
    }
}

namespace {
void falling_image(Game& game,int slot,SoundId sound=SoundId::ChasmFall) {
    const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (game.fall_count<static_cast<int>(game.falls.size()))
        game.falls[static_cast<std::size_t>(game.fall_count++)]={
            {slot,actor.generation},actor.cell,actor.kind==EntityKind::GroundItem ? item_sprite(actor.ground_item) : actor.sprite};
    emit_sound(game,sound,actor.cell);
}
}

bool gap_flyer(const Entity& actor) {
    if (actor.health<=0) return false;
    switch (actor.kind) {
    case EntityKind::Bat: case EntityKind::FrostBat: case EntityKind::Owl:
    case EntityKind::Wasp: case EntityKind::Mosquito: case EntityKind::Woodpecker:
    case EntityKind::LanternMoth: case EntityKind::FurnaceMoth: case EntityKind::CarrionCrow:
        return true;
    default: return false;
    }
}

bool navigable_ground(const Entity& actor,const Tile& tile) {
    return walkable(tile) || (!prop_blocks(tile.prop) &&
        ((open_drop(tile.kind) && gap_flyer(actor)) || (tile.kind==TileKind::DeepRiver && river_swimmer(actor))));
}

// Death over a void loses the body and its cargo. Never create shore loot,
// toppled survivors, or a machine explosion on the missing floor.
void remove_unsupported_body(Game& game,int slot,SoundId sound) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    falling_image(game,slot,sound);
    if (actor.kind==EntityKind::Player) {
        cancel_item_action(actor);clear_sled_links(game,actor);
        actor.toss={};actor.vitals={};actor.impassable=false;
        actor.sprite=Sprite::PlayerDead;actor.spawn_wait=180;
        actor.light={};actor.self_light={};
    } else remove_entity(game,{slot,actor.generation});
}

bool finish_chasm_death(Game& game,int slot) {
    const auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health>0 || game.stage.at_or_border(actor.cell).kind!=TileKind::Chasm) return false;
    remove_unsupported_body(game,slot,SoundId::ChasmFall);return true;
}

bool chasm_contact(Game& game,int slot) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.kind==EntityKind::None || actor.toss.ticks>0 ||
        game.stage.at_or_border(actor.cell).kind!=TileKind::Chasm || gap_flyer(actor)) return false;
    if (actor.kind==EntityKind::Projectile && !grounded_projectile(actor)) return false;
    if (actor.kind==EntityKind::GroundItem || actor.kind==EntityKind::Coins || actor.kind==EntityKind::Projectile) {
        falling_image(game,slot);remove_entity(game,{slot,actor.generation});return true;
    }
    if (actor.health<=0) return false;
    // Missing support also removes fixtures immune to ordinary combat damage.
    actor.health=0;
    return finish_chasm_death(game,slot);
}

void step_chasm_contacts(Game& game) {
    for (int slot=0;slot<max_entities;++slot) chasm_contact(game,slot);
}
