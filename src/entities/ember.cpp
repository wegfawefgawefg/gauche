#include "behavior.hpp"
#include "dispatch.hpp"

void init_ember(Entity& ember) {
    ember.sprite = Sprite::Ember;
    ember.light = {5, 960, {255, 74, 36}};
    ember.self_light = {255, 115, 77};
    ember.health = ember.max_health = 65;
    ember.move_interval = 20;
    ember.attack_interval = 55;
    ember.impassable = true;
    insert_item(ember.inventory, make_item(ItemKind::Pistol));
}

void step_ember(Game& game, int slot) {
    Entity& ember = game.entities[static_cast<std::size_t>(slot)];
    if (ember.inventory.held()->loaded == 0) reload_held_item(game, slot);
    const auto target = enemy_target(game, ember.cell, 8);
    if (!target) { wander(game, slot); return; }
    if (ember.attack_wait == 0) {
        const Cell difference = target->cell - ember.cell;
        if (difference.x == 0 || difference.y == 0) {
            ember.facing = difference.x == 0 ?
                           Cell{0, difference.y > 0 ? 1 : -1} :
                           Cell{difference.x > 0 ? 1 : -1, 0};
            use_held_item(game, slot, target->cell);
            ember.attack_wait = ember.attack_interval;
        }
    }
    if (distance(ember.cell, target->cell) > 4) approach(game, slot, target->cell);
}
