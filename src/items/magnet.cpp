#include "magnet.hpp"
#include "sled.hpp"
#include "action.hpp"
#include "../world/floating_items.hpp"
#include "../projectiles/projectile.hpp"

namespace {
constexpr RegionalItem magnet{"Horseshoe Magnet",
    "Hold to reel steel. Carrying it lures cranes. Move freely; 75 pulls, one tile per 0.2s. Copper/wood ignore it.",
    Sprite::HorseshoeMagnet,{1,5,0,0,12,PatternEffect::Utility,true},
    ItemAction::Material,18,1,false,75,0,0,0,0,SoundId::MagnetPull};

bool blocked(const Game& game,Cell cell,int owner) {
    if (projectile_blocked(game,cell)) return true;
    for (int i=0;i<max_entities;++i) {
        const Entity& actor=game.entities[static_cast<std::size_t>(i)];
        if (i!=owner && actor.kind!=EntityKind::None && actor.impassable && actor.cell==cell) return true;
    }
    return false;
}
bool occupied(const Game& game,Cell cell,int cargo) {
    for (int i=0;i<max_entities;++i) {
        const Entity& actor=game.entities[static_cast<std::size_t>(i)];
        if (i!=cargo && actor.kind==EntityKind::GroundItem && actor.cell==cell && actor.ground_item.count>0) return true;
    }
    return false;
}
}
const RegionalItem* magnet_item(ItemKind kind) { return kind==ItemKind::HorseshoeMagnet ? &magnet : nullptr; }

// MATERIAL: This is a shared interaction property, not another loot table.
// Only a substantial iron/steel body or head qualifies. Copper, brass bells,
// stone, wooden gear and cosmetic scrap deliberately do not become cargo.
bool magnetic_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::Pistol: case ItemKind::Musket: case ItemKind::RocketLauncher:
    case ItemKind::Shotgun: case ItemKind::SMG: case ItemKind::Blunderbuss:
    case ItemKind::Ammo: // The supply is carried in a steel ammo box.
    case ItemKind::BearTrap: case ItemKind::Mine: case ItemKind::Pickaxe:
    case ItemKind::Hatchet: case ItemKind::Rake: case ItemKind::DiggingClaws:
    case ItemKind::RopeHook: case ItemKind::SpringTrap: case ItemKind::ReflectingPan:
    case ItemKind::Chisel: case ItemKind::SnowScoop: case ItemKind::SkateBlade:
    case ItemKind::Crampons: case ItemKind::GroundingSpike: case ItemKind::HarpoonGun:
    case ItemKind::EmergencyDoorstop: case ItemKind::TuningFork: case ItemKind::IceAnchor:
    case ItemKind::FuseScissors: case ItemKind::PressHammer: case ItemKind::RivetGun:
    case ItemKind::ChainHook: case ItemKind::BoltPouch: case ItemKind::EmergencyFoam: case ItemKind::TensionSpring: case ItemKind::PocketDrill: case ItemKind::CoolantCan: case ItemKind::FoldingBarricade: case ItemKind::BeltCrank: case ItemKind::HorseshoeMagnet: return true;
    default: return false;
    }
}
bool pull_magnetic_item(Game& game,int slot,Cell direction) {
    Entity& owner=game.entities[static_cast<std::size_t>(slot)];
    if (distance({},direction)!=1 || owner.inventory.held()->uses<=0) return false;
    const int reach=item_pattern(*owner.inventory.held()).maximum;
    for (int step=1;step<=reach;++step) {
        const Cell cell=owner.cell+Cell{direction.x*step,direction.y*step};
        if (blocked(game,cell,slot)) return false;
        for (int i=0;i<max_entities;++i) {
            Entity& cargo=game.entities[static_cast<std::size_t>(i)];
            if (cargo.kind!=EntityKind::GroundItem || cargo.cell!=cell || cargo.ground_item.count<=0 ||
                cargo.ground_item.flight.slot>=0 || !magnetic_item(cargo.ground_item.kind) || sled_cargo(game,cargo)) continue;
            const Cell next=cell-direction;
            // The nearest responsive piece wins. Other loot can jam its landing;
            // never skip through it or merge partial magazines/condition.
            if (blocked(game,next,slot) || occupied(game,next,i)) return false;
            stop_item_float(game,cargo);
            cargo.cell=next;
            // Local presentation records actual displacement, not an electric hit.
            if (game.shot_count<static_cast<int>(game.shots.size())) {
                ShotEvent& event=game.shots[static_cast<std::size_t>(game.shot_count++)];
                event={};event.source=cell;event.end=next;event.magnetic=true;
            }
            return true;
        }
    }
    return false;
}
bool step_magnet(Game& game,int slot,const Input& input) {
    Entity& owner=game.entities[static_cast<std::size_t>(slot)];
    if (owner.inventory.held()->kind!=ItemKind::HorseshoeMagnet) return false;
    if (owner.label_b!=0) cancel_item_action(owner);
    // No target ownership persists between beats: moving, pickup, another hook
    // or another magnet leaves the exact real item at its latest cell.
    if (input.use && !input.cancel_use && !input.drop && !input.interact && !input.reload)
        use_held_item(game,slot,owner.cell+owner.facing);
    return true;
}
