#include "lava.hpp"
#include "water.hpp"
#include <algorithm>

void contact_lava(Game& game,int slot,bool arrived) {
    Entity& actor=game.entities[static_cast<std::size_t>(slot)];
    if (!wading_actor(actor) || game.stage.at_or_border(actor.cell).kind!=TileKind::Lava) return;
    switch (actor.kind) {
    case EntityKind::SlagSnail: case EntityKind::WalkingKiln:
    case EntityKind::Ember: case EntityKind::SteamLeech: return;
    default: break;
    }
    const bool catching=actor.scorch_ticks==0;
    actor.scorch_ticks=std::max(actor.scorch_ticks,300);
    actor.freeze_ticks=0;
    // The ordinary burn supplies flames and a short afterburn on dry ground.
    // Contact damage is additional, cannot be shield-blocked, and never waits
    // for the global exposure pulse when walking, shoving or landing here.
    if (arrived || catching || game.tick%30==0) {
        emit_sound(game,SoundId::LavaSizzle,actor.cell);
        damage_entity(game,slot,5,actor.cell,false);
    }
}
