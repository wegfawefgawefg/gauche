#include "dispatch.hpp"

void init_fixture(Entity& entity) {
    entity.health = entity.max_health = 1;
    switch (entity.kind) {
    case EntityKind::Key: entity.sprite = Sprite::Key; break;
    case EntityKind::Door:
        entity.sprite = Sprite::Door;
        entity.impassable = entity.hard_blocker = true;
        break;
    case EntityKind::Exit: entity.sprite = Sprite::Exit; break;
    case EntityKind::Trap: entity.sprite = Sprite::BearTrap; break;
    case EntityKind::Switch: entity.sprite = Sprite::Switch; break;
    case EntityKind::Campfire: entity.sprite = Sprite::Campfire; break;
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
