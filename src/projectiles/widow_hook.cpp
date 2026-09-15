#include "widow_hook.hpp"
#include "../entities/fishing_widow.hpp"
#include "../combat/parry.hpp"
#include "../combat/shove.hpp"

namespace {

void finish(Game& game, int slot, bool snapped) {
    const Entity& hook = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot,hook.generation};
    Entity* widow = get_entity(game,hook.entity_a);
    if (snapped) emit_sound(game,SoundId::WidowSnap,hook.cell);
    if (widow && widow->health > 0 && widow->label_a == WidowLine && widow->entity_a == handle) {
        interrupt_fishing_widow(*widow);
        emit_sound(game,SoundId::WidowTangle,widow->cell);
    }
    remove_entity(game,handle);
}

bool line_clear(const Game& game, const Entity& hook) {
    const int length = distance(hook.point_a,hook.cell);
    if (length > hook.attack_interval || hook.cell != hook.point_a +
        Cell{hook.facing.x*length,hook.facing.y*length}) return false;
    for (int i=1;i<=length;++i) {
        const Cell cell=hook.point_a+Cell{hook.facing.x*i,hook.facing.y*i};
        if (projectile_blocked(game,cell)) return false;
        // A body crossing an established line cuts it; the caught body is its endpoint.
        for (int slot=0;slot<max_entities;++slot) {
            const Entity& actor=game.entities[static_cast<std::size_t>(slot)];
            if (actor.kind == EntityKind::None || !actor.impassable || actor.cell != cell) continue;
            if (hook.label_b == 1 && slot == hook.entity_b.slot && actor.generation == hook.entity_b.generation) continue;
            return false;
        }
    }
    return true;
}

void cast_step(Game& game,int slot) {
    Entity& hook=game.entities[static_cast<std::size_t>(slot)];
    const Cell next=hook.cell+hook.facing;
    if (projectile_blocked(game,next)) { finish(game,slot,true); return; }
    hook.cell=next;
    hook.timer_b=4;
    const int victim=entity_at(game,next,true);
    if (victim >= 0) {
        Entity& actor=game.entities[static_cast<std::size_t>(victim)];
        if (actor.health <= 0 || actor.hard_blocker || actor.move_interval <= 0 ||
            parry_ranged_hit(game,victim,hook.facing)) { finish(game,slot,true); return; }
        const bool shield=blocks_facing(actor,hook.point_a);
        const int before=actor.health;
        damage_entity(game,victim,14,hook.point_a,true,hook.entity_a);
        // A blocked or depth-avoided hit must not secretly attach a tether.
        if (shield || actor.health <= 0 || actor.health >= before) { finish(game,slot,true); return; }
        hook.entity_b={victim,actor.generation};
        hook.label_b=1;
        hook.counter_a=0;
        hook.timer_b=10;
        emit_sound(game,SoundId::WidowLatch,next);
        return;
    }
    if (--hook.counter_a == 0) finish(game,slot,false);
}

void reel_step(Game& game,int slot) {
    Entity& hook=game.entities[static_cast<std::size_t>(slot)];
    Entity* victim=get_entity(game,hook.entity_b);
    if (!victim || victim->health <= 0 || victim->hard_blocker || victim->move_interval <= 0) {
        finish(game,slot,true); return;
    }
    hook.cell=victim->cell;
    if (!line_clear(game,hook)) { finish(game,slot,true); return; }
    if (distance(hook.point_a,hook.cell) <= 1) { finish(game,slot,false); return; }
    if (hook.timer_b > 0) return;
    const Cell toward{-hook.facing.x,-hook.facing.y};
    const Cell next=hook.cell+toward;
    const Tile* tile=game.stage.at(next);
    // PULL: Cover breaks the line; this tether cannot crush someone through a wall.
    if (!tile || !walkable(*tile) || entity_at(game,next,true) >= 0 ||
        !shove_actor(game,hook.entity_b.slot,toward,hook.point_a)) { finish(game,slot,true); return; }
    hook.cell=victim->cell;
    hook.timer_b=10;
    emit_sound(game,SoundId::WidowReel,hook.cell);
}

} // namespace

// SLOTS: entity_a caster; entity_b hooked actor; point_a fixed origin; facing fixed lane.
// label_b flight/caught; counter_a outbound range; timer_a finite life; timer_b flight/pull beat.
Handle launch_widow_hook(Game& game,int owner_slot) {
    const Entity& owner=game.entities[static_cast<std::size_t>(owner_slot)];
    const Handle handle=spawn_entity(game,EntityKind::Projectile,owner.cell);
    Entity* hook=get_entity(game,handle);
    if (!hook) return {};
    hook->label_a=static_cast<int>(ProjectileKind::WidowHook);
    hook->entity_a={owner_slot,owner.generation};
    hook->point_a=owner.cell;
    hook->facing=owner.facing;
    hook->counter_a=hook->attack_interval=owner.counter_a;
    hook->timer_a=120; hook->timer_b=4;
    hook->sprite=Sprite::WidowHook;
    return handle;
}

void step_widow_hook(Game& game,int slot) {
    Entity& hook=game.entities[static_cast<std::size_t>(slot)];
    const Entity* owner=get_entity(game,hook.entity_a);
    if (!owner || owner->kind != EntityKind::FishingWidow || owner->health <= 0 ||
        owner->sleep_ticks > 0 || owner->stun_ticks > 0 || owner->cell != hook.point_a ||
        owner->label_a != WidowLine || owner->entity_a != Handle{slot,hook.generation} || hook.timer_a == 0) {
        finish(game,slot,true); return;
    }
    if (hook.label_b != 0) { reel_step(game,slot); return; }
    if (!line_clear(game,hook)) { finish(game,slot,true); return; }
    if (hook.timer_b == 0) cast_step(game,slot);
}
