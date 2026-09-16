#include "water.hpp"
#include "chasm.hpp"
#include "floating_items.hpp"
#include "../entities/river_raft.hpp"

bool river_swimmer(const Entity& actor) {
    return actor.health>0 && (actor.kind==EntityKind::GlassEel ||
        actor.kind==EntityKind::SealThief || actor.kind==EntityKind::BellDiver);
}

bool finish_deep_river_death(Game& game,int slot) {
    const auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.health>0 || game.stage.at_or_border(actor.cell).kind!=TileKind::DeepRiver) return false;
    remove_unsupported_body(game,slot,SoundId::BridgeSplash);return true;
}

bool deep_river_contact(Game& game,int slot) {
    auto& actor=game.entities[static_cast<std::size_t>(slot)];
    if (actor.kind==EntityKind::None || actor.toss.ticks>0 ||
        game.stage.at_or_border(actor.cell).kind!=TileKind::DeepRiver || gap_flyer(actor) || river_swimmer(actor)) return false;
    if (actor.kind==EntityKind::Projectile && !grounded_projectile(actor)) return false;
    if (actor.kind==EntityKind::GroundItem && (actor.ground_item.flight.slot>=0 ||
        actor.ground_item.kind==ItemKind::RiverFish || floating_item(actor))) return false;
    if (actor.kind==EntityKind::RiverRaft && actor.health>0) return false;
    // Acquire an available support on arrival, before resolving drowning. This
    // also handles teleports, toss landings and cargo spawned after its raft.
    catch_river_raft(game,slot);
    if (ridden_river_raft(game,actor)) return false;
    if (actor.kind==EntityKind::Player && actor.health<=0) return false;
    actor.health=0;
    return finish_deep_river_death(game,slot);
}

void step_deep_river_contacts(Game& game) {
    for (int slot=0;slot<max_entities;++slot) deep_river_contact(game,slot);
}
