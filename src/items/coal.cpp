#include "coal.hpp"
#include "../entities/boiler_tank.hpp"
#include "../props/stove.hpp"
#include "../projectiles/recoverable.hpp"

namespace {
constexpr RegionalItem coal{"Coal Lump", "Stove / boiler: +20s fuel, cap 120s. Cold stoves need ignition. Otherwise throw a recoverable lump.",
    Sprite::CoalLump,{1,6,0,4,30,PatternEffect::Damage,true},
    ItemAction::Throw,3,8,true,0,0,0,0,0,SoundId::CoalThrow};
}

const RegionalItem* coal_item(ItemKind kind) { return kind == ItemKind::CoalLump ? &coal : nullptr; }

bool use_coal(Game& game, int slot, Cell direction) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    const Cell target = actor.cell+direction;
    if (boiler_at(game,target) >= 0) return feed_boiler(game,target);
    const Prop& prop = game.stage.at_or_border(target).prop;
    // FULL: A full stove rejects fuel, rather than making a surprise throw at it.
    if (prop.kind == PropKind::Stove && !prop.broken) return feed_stove(game,target);
    if (!launch_recoverable(game,slot,*actor.inventory.held(),direction)) return false;
    emit_sound(game,SoundId::CoalThrow,actor.cell);
    return true;
}
