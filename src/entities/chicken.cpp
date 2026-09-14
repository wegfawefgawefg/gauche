#include "behavior.hpp"
#include "dispatch.hpp"

// SLOTS: label_a = chick/hen/rooster; entity_a = adult leader; entity_b = threat.
// point_a = home; point_b = last threat location; timer_a = panic; timer_b = flock search.
void init_chicken(Game& game, Entity& entity) {
    const int variant = static_cast<int>(random_u32(game) % 4);
    entity.label_a = variant < 2 ? 0 : variant - 1;
    entity.point_a = entity.cell;
    entity.sprite = variant < 2 ? Sprite::Chick :
                    (variant == 2 ? Sprite::Hen : Sprite::Rooster);
    entity.health = entity.max_health = variant < 2 ? 1 : (variant == 2 ? 3 : 30);
    entity.move_interval = variant < 2 ? 18 : (variant == 2 ? 30 : 42);
    entity.move_wait = static_cast<int>(random_u32(game) %
                                        static_cast<std::uint32_t>(entity.move_interval));
    entity.impassable = true;
}

void step_chicken(Game& game, int slot) {
    Entity& chicken = game.entities[static_cast<std::size_t>(slot)];
    const Entity* leader = get_entity(game, chicken.entity_a);
    if (chicken.label_a == 0 && (leader == nullptr || leader->kind != EntityKind::Chicken ||
        leader->health <= 0 || leader->label_a == 0) && chicken.timer_b == 0) {
        chicken.entity_a = {};
        int closest = 11;
        for (int candidate = 0; candidate < max_entities; ++candidate) {
            const Entity& adult = game.entities[static_cast<std::size_t>(candidate)];
            if (adult.kind != EntityKind::Chicken || adult.label_a == 0 || adult.health <= 0)
                continue;
            const int gap = distance(adult.cell, chicken.cell);
            if (gap >= closest) continue;
            chicken.entity_a = {candidate, adult.generation};
            closest = gap;
        }
        leader = get_entity(game, chicken.entity_a);
        chicken.timer_b = 60;
    }
    if (chicken.timer_a > 0) {
        if (const Entity* threat = get_entity(game, chicken.entity_b))
            chicken.point_b = threat->cell;
        flee(game, slot, chicken.point_b);
    } else if (leader != nullptr && leader->kind == EntityKind::Chicken && leader->health > 0) {
        if (distance(chicken.cell, leader->cell) > 1) pursue(game, slot, leader->cell);
    } else if (distance(chicken.cell, chicken.point_a) > 7) approach(game, slot, chicken.point_a);
    else wander(game, slot);
    const SoundId call = chicken.label_a == 0 ? SoundId::Chick :
                         (chicken.label_a == 1 ? SoundId::Hen : SoundId::Rooster);
    maybe_growl(game, slot, call);
}

void spawn_chicken_family(Game& game, Cell cell) {
    const Handle mother = spawn_entity(game, EntityKind::Chicken, cell);
    Entity* hen = get_entity(game, mother);
    if (hen == nullptr) return;
    hen->label_a = 1;
    hen->sprite = Sprite::Hen;
    hen->health = hen->max_health = 3;
    hen->move_interval = 30;
    constexpr Cell sides[]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (Cell side : sides) {
        const Cell nest = cell + side;
        const Tile* tile = game.stage.at(nest);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, nest, false) >= 0) continue;
        Entity* chick = get_entity(game, spawn_entity(game, EntityKind::Chicken, nest));
        if (chick == nullptr) break;
        chick->label_a = 0;
        chick->sprite = Sprite::Chick;
        chick->health = chick->max_health = 1;
        chick->move_interval = 18;
        chick->entity_a = mother;
    }
}
