#include "dispatch.hpp"

void init_fixture(Entity& entity) {
    entity.health = entity.max_health = 1;
    switch (entity.kind) {
    case EntityKind::Key:
        entity.sprite = Sprite::Key;
        entity.light = {3, 560, {255, 212, 71}};
        entity.self_light = {242, 209, 97};
        break;
    case EntityKind::Door:
        entity.sprite = Sprite::Door;
        entity.impassable = entity.hard_blocker = true;
        break;
    case EntityKind::Exit:
        entity.sprite = Sprite::Exit;
        entity.light = {6, 950, {61, 186, 232}};
        break;
    case EntityKind::EncounterGate:
        entity.sprite = Sprite::EncounterGate;
        entity.fixture_open = true;
        break;
    case EntityKind::WaveVent:
        entity.sprite = Sprite::GraveVent;
        entity.light = {2, 120, {157, 117, 190}};
        break;
    case EntityKind::Trap: entity.sprite = Sprite::BearTrap; break;
    case EntityKind::Switch: entity.sprite = Sprite::Switch; break;
    case EntityKind::Campfire:
        entity.sprite = Sprite::Campfire;
        entity.light = {8, 1400, {255, 140, 59}};
        entity.self_light = {255, 209, 153};
        break;
    case EntityKind::Crusher:
        entity.sprite = Sprite::Crusher;
        entity.health = entity.max_health = 1000000;
        entity.impassable = entity.hard_blocker = true;
        entity.move_interval = entity.move_wait = 12;
        entity.facing = {1, 0};
        break;
    default: break;
    }
}
