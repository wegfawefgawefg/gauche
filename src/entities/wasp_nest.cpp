#include "dispatch.hpp"
#include "behavior.hpp"

#include <algorithm>

// SLOTS: counter_a remaining brood; counter_b awake brood beat; entity_a last child.
// Each child owns a nest handle and a preceding-member handle. Brood is finite.
void init_wasp_nest(Entity& nest) {
    nest.sprite = Sprite::WaspNest;
    nest.health = nest.max_health = 70;
    nest.impassable = nest.hard_blocker = true;
    nest.counter_a = 6;
    nest.counter_b = 150;
}

void step_wasp_nest(Game& game, int slot) {
    Entity& nest = game.entities[static_cast<std::size_t>(slot)];
    if (nest.counter_a == 0) { nest.sprite = Sprite::WaspNestEmpty; return; }
    const Surface& air = game.stage.at_or_border(nest.cell).surface;
    // SMOKE: Restart the warning after suppression. Sleeping skips this entire beat.
    if (air.smoke_ticks > 0 || air.sleep_ticks > 0) {
        nest.counter_b = std::max(nest.counter_b, 90);
        nest.sprite = Sprite::WaspNest;
        return;
    }
    const Handle home{slot, nest.generation};
    int brood = 0;
    for (const Entity& bug : game.entities)
        if (bug.kind == EntityKind::Wasp && bug.health > 0 && bug.entity_b == home) ++brood;
    if (brood >= 3 || nearest_player(game, nest.cell, 9) < 0) return;
    if (nest.counter_b > 0) --nest.counter_b;
    if (nest.counter_b == 40) emit_sound(game, SoundId::NestBuzz, nest.cell);
    nest.sprite = nest.counter_b <= 40 ? Sprite::WaspNestStirring : Sprite::WaspNest;
    if (nest.counter_b > 0) return;
    constexpr Cell sides[]{{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    const unsigned int start = random_u32(game) % 4;
    for (unsigned int i = 0; i < 4; ++i) {
        const Cell cell = nest.cell + sides[(start + i) % 4];
        const Tile* tile = game.stage.at(cell);
        if (tile == nullptr || !walkable(*tile) || entity_at(game, cell, true) >= 0) continue;
        const Handle handle = spawn_entity(game, EntityKind::Wasp, cell);
        Entity* child = get_entity(game, handle);
        if (child == nullptr) { nest.counter_b = 30; return; }
        const Entity* preceding = get_entity(game, nest.entity_a);
        child->entity_a = preceding != nullptr && preceding->health > 0 ? nest.entity_a : home;
        child->entity_b = home;
        child->point_a = nest.cell;
        child->timer_a = 15 + brood * 12;
        child->label_a = 2;
        child->encounter = nest.encounter;
        nest.entity_a = handle;
        --nest.counter_a;
        nest.counter_b = 150;
        nest.sprite = nest.counter_a == 0 ? Sprite::WaspNestEmpty : Sprite::WaspNest;
        emit_sound(game, SoundId::WaspRelease, nest.cell);
        return;
    }
    nest.counter_b = 12;
}
