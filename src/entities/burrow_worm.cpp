#include "dispatch.hpp"
#include "behavior.hpp"
#include "following.hpp"
#include "attacks.hpp"

#include <algorithm>

namespace {

void expose_head(Game& game, Entity& worm) {
    // CUT: The next live link becomes a head in place, retaining wounds and effects.
    worm.entity_a = {};
    worm.label_a = 0; worm.label_b = 2;
    worm.timer_a = 50; worm.move_interval = 14;
    worm.sprite = Sprite::BurrowWormHead;
    emit_sound(game, SoundId::WormSplit, worm.cell);
}

bool tail_ready(const Game& game, const Entity& worm) {
    const Entity* tail = get_entity(game, worm.entity_b);
    return tail == nullptr || tail->health <= 0 || distance(worm.cell, tail->cell) <= 1;
}

void step_head(Game& game, int slot, Entity& worm) {
    if (worm.label_b == 1) {
        if (worm.timer_a > 0) return;
        if (distance(worm.cell, worm.point_a) == 1)
            resolve_enemy_attack(game, slot, 12, SoundId::WormBite);
        worm.label_b = 2; worm.timer_a = 32; worm.sprite = Sprite::BurrowWormHead;
        return;
    }
    if (worm.label_b == 2) {
        if (worm.timer_a == 0) worm.label_b = 0;
        return;
    }
    const auto target = enemy_target(game, worm.cell, 8);
    if (!target) {
        if (tail_ready(game, worm)) wander(game, slot);
        return;
    }
    const Cell destination = target->cell;
    if (distance(worm.cell, destination) == 1) {
        worm.point_a = destination; worm.facing = destination - worm.cell;
        worm.label_b = 1; worm.timer_a = 24; worm.sprite = Sprite::BurrowWormBite;
        emit_sound(game, SoundId::WormWarning, worm.cell);
        return;
    }
    if (worm.move_wait > 0 || !tail_ready(game, worm)) return;
    const Cell next = worm.cell + cardinal_toward(worm.cell, destination, worm.facing);
    const Tile* tile = game.stage.at(next);
    // TUNNEL: Rumble and chew eligible material. One ordinary dig strike per beat;
    // hard boundaries stay hard and a tunnel opens for every other actor too.
    if (tile != nullptr && tile->kind == TileKind::Wall && tile->break_rule != BreakRule::Unbreakable &&
        (tile->break_rule != BreakRule::DigRequired || tile->required_dig_power <= 1)) {
        worm.facing = next - worm.cell;
        if (worm.timer_a > 0) return;
        if (worm.counter_a == 0) {
            worm.counter_a = 1; worm.timer_a = 36;
            emit_sound(game, SoundId::WormBurrow, worm.cell);
        } else {
            hit_terrain(game, next, worm.cell, 16, 1);
            worm.timer_a = 18;
        }
        return;
    }
    worm.counter_a = 0;
    pursue(game, slot, destination);
}

} // namespace

// SLOTS: label_a head/body; label_b seek/bite/recover; point_a committed bite;
// point_b/timer_b departed trail; entity_a previous link, entity_b next link;
// timer_a bite/dig/recovery beat; counter_a digging warning latch.
void init_burrow_worm(Entity& worm) {
    worm.sprite = Sprite::BurrowWormHead;
    worm.health = worm.max_health = 55;
    worm.move_interval = 14;
    worm.impassable = true;
    worm.point_b = worm.cell;
}

void step_burrow_worm(Game& game, int slot) {
    Entity& worm = game.entities[static_cast<std::size_t>(slot)];
    const Cell previous = worm.cell;
    if (worm.label_a == 1) {
        const Entity* leader = get_entity(game, worm.entity_a);
        if (leader == nullptr || leader->kind != EntityKind::BurrowWorm || leader->health <= 0)
            expose_head(game, worm);
        else if (tail_ready(game, worm)) follow_trail(game, slot, *leader);
    } else step_head(game, slot, worm);
    record_trail(worm, previous, 2);
}

Handle spawn_burrow_worm(Game& game, Cell cell, int links) {
    const Handle head = spawn_entity(game, EntityKind::BurrowWorm, cell);
    Handle preceding = head;
    constexpr Cell sides[]{{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    unsigned int direction = random_u32(game) % 4;
    for (int link = 0; link < links; ++link) {
        Entity* leader = get_entity(game, preceding);
        if (leader == nullptr) break;
        bool placed = false;
        for (unsigned int i = 0; i < 4; ++i) {
            const Cell next = leader->cell + sides[(direction + i) % 4];
            const Tile* tile = game.stage.at(next);
            if (tile == nullptr || !walkable(*tile) || entity_at(game, next, false) >= 0) continue;
            const Handle handle = spawn_entity(game, EntityKind::BurrowWorm, next);
            Entity* child = get_entity(game, handle);
            if (child == nullptr) return head;
            child->entity_a = preceding; leader->entity_b = handle;
            child->label_a = 1; child->health = child->max_health = 26;
            child->sprite = Sprite::BurrowWormBody;
            child->move_interval = 5; child->move_wait = 3 + link * 2;
            child->facing = leader->cell - child->cell;
            direction = (direction + i) % 4;
            if (link == 0) leader->facing = leader->cell - next;
            preceding = handle; placed = true;
            break;
        }
        if (!placed) break;
    }
    return head;
}
