#include "behavior.hpp"
#include "dispatch.hpp"

void init_wildlife(Game&, Entity& entity) {
    entity.impassable = true;
    switch (entity.kind) {
    case EntityKind::Bat: case EntityKind::FrostBat:
        entity.sprite = entity.kind == EntityKind::Bat ? Sprite::Bat : Sprite::FrostBat;
        entity.health = entity.max_health = 18;
        entity.move_interval = 15;
        entity.attack_interval = 45;
        break;
    case EntityKind::Wolf:
        entity.sprite = Sprite::Wolf;
        entity.health = entity.max_health = 45;
        entity.move_interval = 10;
        entity.attack_interval = 38;
        break;
    case EntityKind::Bear:
        entity.sprite = Sprite::Bear;
        entity.health = entity.max_health = 140;
        entity.move_interval = 25;
        entity.attack_interval = 55;
        break;
    case EntityKind::Bunny:
        entity.sprite = Sprite::Bunny;
        entity.health = entity.max_health = 5;
        entity.move_interval = 14;
        break;
    case EntityKind::Ember:
        entity.sprite = Sprite::Ember;
        entity.health = entity.max_health = 65;
        entity.move_interval = 20;
        entity.attack_interval = 55;
        insert_item(entity.inventory, make_item(ItemKind::Pistol));
        break;
    default: break;
    }
}

namespace {

void step_hunter(Game& game, int slot) {
    Entity& enemy = game.entities[static_cast<std::size_t>(slot)];
    const int target_slot = nearest_player(game, enemy.cell, 9);
    if (target_slot < 0) wander(game, slot);
    else {
        const Cell target = game.entities[static_cast<std::size_t>(target_slot)].cell;
        if (distance(enemy.cell, target) > 1) approach(game, slot, target);
    }
    const int damage = enemy.kind == EntityKind::Bear ? 24 :
                       (enemy.kind == EntityKind::Wolf ? 11 : 7);
    bite(game, slot, damage);
}

void step_ember(Game& game, int slot) {
    Entity& ember = game.entities[static_cast<std::size_t>(slot)];
    if (ember.inventory.held()->loaded == 0) reload_held_item(game, slot);
    const int target_slot = nearest_player(game, ember.cell, 8);
    if (target_slot < 0) { wander(game, slot); return; }
    const Entity& target = game.entities[static_cast<std::size_t>(target_slot)];
    if (ember.attack_wait == 0) {
        const Cell difference = target.cell - ember.cell;
        if (difference.x == 0 || difference.y == 0) {
            ember.facing = difference.x == 0 ?
                           Cell{0, difference.y > 0 ? 1 : -1} :
                           Cell{difference.x > 0 ? 1 : -1, 0};
            use_held_item(game, slot, target.cell);
            ember.attack_wait = ember.attack_interval;
        }
    }
    if (distance(ember.cell, target.cell) > 4) approach(game, slot, target.cell);
}

} // namespace

void step_wildlife(Game& game, int slot) {
    const EntityKind kind = game.entities[static_cast<std::size_t>(slot)].kind;
    if (kind == EntityKind::Bunny) wander(game, slot);
    else if (kind == EntityKind::Ember) step_ember(game, slot);
    else step_hunter(game, slot);
}
