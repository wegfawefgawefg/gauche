#include "behavior.hpp"
#include "dispatch.hpp"
#include "following.hpp"
#include "bird_feeding.hpp"

#include <algorithm>

// SLOTS: label_a = chick/hen/rooster; entity_a = preceding bird; entity_b = threat.
// point_a = home; point_b/timer_b = delayed trail; timer_a = alarm memory.
// counter_a/b = last threat x/y; counter_c = trail yield lifetime.
void init_chicken(Game& game, Entity& entity) {
    const int variant = static_cast<int>(random_u32(game) % 4);
    entity.label_a = variant < 2 ? 0 : variant - 1;
    entity.point_a = entity.point_b = entity.cell;
    entity.sprite = variant < 2 ? Sprite::Chick :
                    (variant == 2 ? Sprite::Hen : Sprite::Rooster);
    entity.health = entity.max_health = variant < 2 ? 1 : 45;
    entity.move_interval = variant < 2 ? 9 : (variant == 2 ? 30 : 42);
    entity.move_wait = static_cast<int>(random_u32(game) %
                                        static_cast<std::uint32_t>(entity.move_interval));
    entity.impassable = true;
}

namespace {

void defend_family(Game& game, int slot, Entity& chicken) {
    Entity* threat = get_entity(game, chicken.entity_b);
    const Cell target{chicken.counter_a, chicken.counter_b};
    if (distance(chicken.cell, target) > 10) return;
    if (threat == nullptr || threat->health <= 0) { chicken.timer_a = 0; return; }
    chicken.facing = cardinal_toward(chicken.cell, threat->cell, chicken.facing);
    if (distance(chicken.cell, threat->cell) > 1) pursue(game, slot, threat->cell);
    else if (chicken.attack_wait == 0) {
        damage_entity(game, chicken.entity_b.slot, chicken.label_a == 1 ? 2 : 4, chicken.cell);
        chicken.attack_wait = 28;
        chicken.use_flash = 8;
        emit_sound(game, SoundId::ChickenPeck, chicken.cell);
    }
}

} // namespace

void step_chicken(Game& game, int slot) {
    Entity& chicken = game.entities[static_cast<std::size_t>(slot)];
    if (chicken.counter_c > 0) --chicken.counter_c;
    const Entity* threat = get_entity(game, chicken.entity_b);
    if (threat != nullptr && threat->health > 0) {
        chicken.counter_a = threat->cell.x;
        chicken.counter_b = threat->cell.y;
        if (distance(chicken.cell, threat->cell) <= 7)
            chicken.timer_a = std::max(chicken.timer_a, 120);
    }
    if (chicken.timer_a == 0) chicken.entity_b = {};
    const bool scared = chicken.label_a == 0 && chicken.timer_a > 0;
    chicken.move_interval = scared ? 5 : chicken.timer_a > 0 ? 10 :
        chicken.label_a == 0 ? 9 : chicken.label_a == 1 ? 30 : 42;
    const Cell previous = chicken.cell;
    if (scared) {
        flee(game, slot, {chicken.counter_a, chicken.counter_b});
    } else if (chicken.timer_a > 0) {
        defend_family(game, slot, chicken);
    } else if (chicken.label_a != 0 && feed_on_bird_seed(game, slot)) {
        // Chicks follow the feeding adult instead of abandoning their chain.
    } else {
        const Entity* leader = get_entity(game, chicken.entity_a);
        if (chicken.label_a == 0 && (leader == nullptr || leader->kind != EntityKind::Chicken || leader->health <= 0)) {
            if ((game.tick + static_cast<std::uint64_t>(slot)) % 60 == 0)
                chicken.entity_a = find_chain_tail(game, slot, EntityKind::Chicken, 12);
            leader = get_entity(game, chicken.entity_a);
        }
        if (leader != nullptr && leader->kind == EntityKind::Chicken && leader->health > 0) {
            if (!yield_trail(game, slot, *leader)) follow_trail(game, slot, *leader);
        } else if (distance(chicken.cell, chicken.point_a) > 7) pursue(game, slot, chicken.point_a);
        else wander(game, slot);
    }
    record_trail(chicken, previous, scared ? 2 : 5);
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
    hen->health = hen->max_health = 45;
    hen->move_interval = 30;
    Handle preceding = mother;
    Cell tail = cell;
    constexpr Cell sides[]{{-1, 0}, {0, 1}, {0, -1}, {1, 0}};
    for (int member = 0; member < 4; ++member) {
        bool placed = false;
        for (Cell side : sides) {
            const Cell nest = tail + side;
            const Tile* tile = game.stage.at(nest);
            if (tile == nullptr || !walkable(*tile) || entity_at(game, nest, false) >= 0) continue;
            const Handle handle = spawn_entity(game, EntityKind::Chicken, nest);
            Entity* chick = get_entity(game, handle);
            if (chick == nullptr) return;
            chick->label_a = 0;
            chick->sprite = Sprite::Chick;
            chick->health = chick->max_health = 1;
            chick->move_interval = 9;
            chick->move_wait = 5 + member * 2;
            chick->entity_a = preceding;
            preceding = handle;
            tail = nest;
            placed = true;
            break;
        }
        if (!placed) break;
    }
}
