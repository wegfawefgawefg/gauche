#include "net.hpp"
#include "../item_pattern.hpp"

namespace {

void finish(Game& game, int slot, Cell cell, SoundId sound) {
    const Entity& net = game.entities[static_cast<std::size_t>(slot)];
    remove_entity(game, {slot, net.generation});
    emit_sound(game, sound, cell);
}

} // namespace

// SLOTS: label_b surviving cloth lanes; counter_a range; attack_interval total range;
// timer_a life, timer_b travel beat. Copied item fixes width/reach after throwing.
bool launch_net(Game& game, int owner_slot, const Item& item, Cell direction) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    Entity* net = get_entity(game, spawn_entity(game, EntityKind::Projectile, owner.cell));
    if (net == nullptr) return false;
    const ItemPattern pattern = item_pattern(item);
    net->label_a = static_cast<int>(ProjectileKind::Net);
    net->label_b = (1 << (pattern.half_width * 2 + 1)) - 1;
    net->counter_a = net->attack_interval = pattern.maximum;
    net->timer_a = pattern.maximum * 4 + 4;
    net->timer_b = 4;
    net->entity_a = {owner_slot, owner.generation};
    net->point_a = owner.cell;
    net->facing = direction;
    net->ground_item = item;
    net->ground_item.count = 1;
    net->sprite = Sprite::NetFlight;
    return true;
}

bool net_target(const Entity& actor) {
    return actor.health > 0 && actor.move_interval > 0 && actor.impassable && !actor.hard_blocker;
}

int net_open_lanes(const Game& game, Cell center, Cell side, int width, int mask) {
    if (projectile_blocked(game, center)) return 0;
    for (int lane = -width; lane <= width; ++lane)
        if (projectile_blocked(game, center + Cell{side.x * lane, side.y * lane}))
            mask &= ~(1 << (lane + width));
    return mask;
}

void step_net(Game& game, int slot) {
    Entity& net = game.entities[static_cast<std::size_t>(slot)];
    if (net.timer_a == 0) { finish(game, slot, net.cell, SoundId::NetFall); return; }
    if (net.timer_b > 0) return;
    const Cell next = net.cell + net.facing;
    const Cell side{-net.facing.y, net.facing.x};
    const int width = item_pattern(net.ground_item).half_width;
    net.label_b = net_open_lanes(game, next, side, width, net.label_b);
    if (net.label_b == 0) { finish(game, slot, net.cell, SoundId::NetFall); return; }
    bool caught = false;
    // CLOTH: Walls tear off edge lanes. Stop at the first row with a moving actor.
    for (int lane = -width; lane <= width; ++lane) {
        const int bit = 1 << (lane + width);
        if ((net.label_b & bit) == 0) continue;
        const Cell cell = next + Cell{side.x * lane, side.y * lane};
        for (int index = 0; index < max_entities; ++index) {
            Entity& actor = game.entities[static_cast<std::size_t>(index)];
            if (index == net.entity_a.slot && actor.generation == net.entity_a.generation) continue;
            if (actor.cell != cell || !net_target(actor)) continue;
            caught |= apply_root(actor, net_hold_ticks, RootKind::Net);
        }
    }
    if (caught) { finish(game, slot, next, SoundId::NetCatch); return; }
    net.cell = next;
    net.timer_b = 4;
    if (--net.counter_a <= 0) finish(game, slot, next, SoundId::NetFall);
}
