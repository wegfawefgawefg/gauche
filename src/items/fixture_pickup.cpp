#include "ground_interaction.hpp"

namespace {
const Entity* fixture_at(const Game& game, Cell cell, bool feet) {
    for (const Entity& fixture:game.entities) {
        if (fixture.cell!=cell) continue;
        switch (fixture.kind) {
        case EntityKind::Key: if (feet) return &fixture; break;
        case EntityKind::Switch:
        case EntityKind::Door: if (!fixture.fixture_open) return &fixture; break;
        case EntityKind::Campfire: if (fixture.fire_tramples<5) return &fixture; break;
        case EntityKind::Exit:
        case EntityKind::Encounter: return &fixture;
        default: break;
        }
    }
    return nullptr;
}
}

const Entity* pickup_fixture(const Game& game, const Entity& player) {
    const Entity* feet=fixture_at(game,player.cell,true);
    if (feet && feet->kind==EntityKind::Key) return feet;
    // Loose items and portable fixtures keep pickup priority over nearby mechanisms.
    if (reachable_pickup_item(game,player).kind!=ItemKind::None) return nullptr;
    return feet ? feet : fixture_at(game,player.cell+player.facing,false);
}

const char* pickup_fixture_label(const Game& game, const Entity& fixture) {
    switch (fixture.kind) {
    case EntityKind::Key: return "TAKE KEY";
    case EntityKind::Switch: return "PULL LEVER";
    case EntityKind::Door: return game.run.has_key ? "UNLOCK DOOR" : "NEED KEY";
    case EntityKind::Exit: return "LEAVE TOGETHER";
    case EntityKind::Encounter: return "ACTIVATE";
    case EntityKind::Campfire: return "USE CAMPFIRE";
    default: return "INTERACT";
    }
}

bool use_pickup_fixture(Game& game, Entity& player) {
    const Entity* fixture=pickup_fixture(game,player);
    if (!fixture) return false;
    const Cell cell=fixture->cell;
    (void)interact_with_fixture(game,player.owner,cell);
    // Even a locked door or a waiting exit consumes the press; never drop by accident.
    return true;
}
