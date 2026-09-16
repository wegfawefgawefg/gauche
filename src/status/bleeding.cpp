#include "bleeding.hpp"
#include <algorithm>

bool can_bleed(EntityKind kind) {
    switch (kind) {
    case EntityKind::Player: case EntityKind::ZombieStack: case EntityKind::Zombie: case EntityKind::Chicken:
    case EntityKind::BurrowWorm:
    case EntityKind::Wasp: case EntityKind::ForagerGoblin: case EntityKind::CarrionCrow:
    case EntityKind::Mosquito: case EntityKind::Owl: case EntityKind::Woodpecker:
    case EntityKind::Ant: case EntityKind::Snake: case EntityKind::ForestSpider: case EntityKind::Bat: case EntityKind::Wolf: case EntityKind::Dog:
    case EntityKind::Bear: case EntityKind::Boar: case EntityKind::SporeToad:
    case EntityKind::ThornSnail: case EntityKind::LanternMoth:
    case EntityKind::ArcWelder: case EntityKind::Yeti: case EntityKind::AvalancheRam: case EntityKind::WhiteoutDrummer: case EntityKind::SealThief: case EntityKind::FishingWidow: case EntityKind::FrozenPilgrim: case EntityKind::EchoHound: case EntityKind::LensWarden: case EntityKind::MirrorKnight: case EntityKind::SnowBurrower: case EntityKind::GlassEel: case EntityKind::IceMason: case EntityKind::BellDiver: case EntityKind::RimeSkater:
    case EntityKind::RivetGunner: case EntityKind::Strikebreaker: case EntityKind::PowderMonkey:
    case EntityKind::PressureRat: case EntityKind::Bunny: case EntityKind::Ember: case EntityKind::FrostBat:
        return true;
    default: return false;
    }
}

bool apply_bleeding(Entity& actor,int ticks) {
    if (actor.health<=0 || ticks<=0 || !can_bleed(actor.kind)) return false;
    if (actor.vitals.bleeding==0) actor.vitals.bleed_wait=60;
    actor.vitals.bleeding=static_cast<std::uint16_t>(std::clamp(std::max(ticks,static_cast<int>(actor.vitals.bleeding)),0,1800));
    return true;
}
void step_bleeding(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];auto& effect=actor.vitals;
    if (!effect.bleeding) return;
    --effect.bleeding;
    if (effect.bleed_wait>0) --effect.bleed_wait;
    if (!effect.bleed_wait) {
        effect.bleed_wait=60;
        damage_entity(game,slot,1,actor.cell,false);
    }
    if (!effect.bleeding) effect.bleed_wait=0;
}
