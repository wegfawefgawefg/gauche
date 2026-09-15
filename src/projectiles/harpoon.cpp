#include "harpoon.hpp"
#include "../entities/attacks.hpp"
#include "../combat/parry.hpp"
#include "../combat/shove.hpp"
#include "../items/muffling.hpp"
#include "../item_pattern.hpp"
#include "../props/interaction.hpp"

namespace {
bool movable(const Entity& actor) {
    return actor.health > 0 && actor.impassable && !actor.hard_blocker && actor.move_interval > 0;
}

bool tether_clear(const Game& game, const Entity& owner, const Entity& shot) {
    return distance(owner.cell,shot.cell) <= shot.attack_interval &&
        clear_attack_sight(game,owner.cell,shot.cell,false);
}

void latch_or_finish(Game& game, Handle handle) {
    Entity* shot = get_entity(game,handle);
    if (!shot) return;
    const Entity* victim = get_entity(game,shot->entity_b);
    if (!victim || !movable(*victim)) { release_harpoon(game,handle); return; }
    shot->label_b = 1; shot->counter_a = 0; shot->timer_b = 10;
    shot->cell = victim->cell;
    emit_sound(game,SoundId::HarpoonLatch,shot->cell);
}

void fly(Game& game, Handle handle) {
    Entity& shot = *get_entity(game,handle);
    const Cell next = shot.cell+shot.facing;
    if (projectile_blocked(game,next)) {
        hit_prop(game,next,shot.counter_b,shot.cell);
        hit_terrain(game,next,shot.cell,shot.counter_b,shot.ground_item.dig_power);
        emit_sound(game,SoundId::HarpoonImpact,shot.cell);
        latch_or_finish(game,handle); return;
    }
    shot.cell = next; shot.timer_b = 3; --shot.counter_a;
    const int slot = entity_at(game,next,true);
    if (slot >= 0 && Handle{slot,game.entities[static_cast<std::size_t>(slot)].generation} != shot.entity_a) {
        Entity& victim = game.entities[static_cast<std::size_t>(slot)];
        if (parry_ranged_hit(game,slot,shot.facing)) { release_harpoon(game,handle); return; }
        const int before = victim.health;
        const bool shield = blocks_facing(victim,next-shot.facing);
        damage_entity(game,slot,shot.counter_b,next-shot.facing);
        emit_sound(game,SoundId::HarpoonImpact,next);
        if (!shield && victim.health < before && movable(victim)) shot.entity_b = {slot,victim.generation};
        if (shot.counter_c == 0 || shield || victim.hard_blocker) { latch_or_finish(game,handle); return; }
    }
    if (shot.counter_a == 0) latch_or_finish(game,handle);
}

void reel(Game& game, Handle handle, const Entity& owner, bool pulling) {
    Entity& shot = *get_entity(game,handle);
    Entity* victim = get_entity(game,shot.entity_b);
    if (!victim || !movable(*victim)) { release_harpoon(game,handle); return; }
    shot.cell = victim->cell;
    if (!tether_clear(game,owner,shot)) { release_harpoon(game,handle); return; }
    if (!pulling || shot.timer_b > 0 || distance(owner.cell,shot.cell) <= 1) return;
    const Cell direction = cardinal_toward(shot.cell,owner.cell,{-shot.facing.x,-shot.facing.y});
    const Cell next = shot.cell+direction;
    const Tile* tile = game.stage.at(next);
    // PULL: Never invoke shove's crush path. Obstacles hold the line; cover can cut it.
    shot.timer_b = 10;
    if (!tile || !walkable(*tile) || entity_at(game,next,true) >= 0 || victim->vitals.grip > 0) return;
    if (shove_actor(game,shot.entity_b.slot,direction,owner.cell)) {
        shot.cell = victim->cell;
        emit_sound(game,SoundId::HarpoonReel,shot.cell);
    }
}
}

// SLOTS: entity_a owner, entity_b last caught living body; label_b flight/caught.
// counter_a remaining cells, counter_b damage, counter_c piercing. timer_b travel/reel beat.
// fixture_open is this tick's held use; consumed after each step. Item.flight owns the line.
bool launch_harpoon(Game& game, int owner_slot, Item& item, Cell direction) {
    if (distance({},direction) != 1 || item.loaded <= 0 || item.flight.slot >= 0) return false;
    Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    const Handle handle = spawn_entity(game,EntityKind::Projectile,owner.cell);
    Entity* shot = get_entity(game,handle);
    if (!shot) return false;
    const ItemPattern pattern = item_pattern(item);
    shot->label_a = static_cast<int>(ProjectileKind::Harpoon);
    shot->sprite = Sprite::HarpoonHead;
    shot->entity_a = {owner_slot,owner.generation};
    shot->point_a = owner.cell; shot->facing = direction;
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->counter_b = pattern.damage;
    shot->counter_c = pattern.piercing || has_artifact(owner,ArtifactKind::AllPiercing) ? 1 : 0;
    shot->timer_b = 3;
    shot->ground_item = item;
    item.flight = handle;
    emit_weapon_sound(game,item,SoundId::HarpoonFire,owner.cell);
    finish_muffled_use(game,item,owner.cell);
    --item.loaded; item.cooldown = pattern.cooldown; owner.use_flash = 6;
    return true;
}

void release_harpoon(Game& game, Handle handle) {
    Entity* shot = get_entity(game,handle);
    if (!shot || shot->kind != EntityKind::Projectile || shot->label_a != static_cast<int>(ProjectileKind::Harpoon)) return;
    if (Entity* owner = get_entity(game,shot->entity_a))
        for (Item& item : owner->inventory.slots)
            if (item.flight == handle) item.flight = {};
    emit_sound(game,SoundId::HarpoonRelease,shot->cell);
    remove_entity(game,handle);
}

void step_harpoon(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const Handle handle{slot,shot.generation};
    const Entity* owner = get_entity(game,shot.entity_a);
    if (!owner || owner->health <= 0 || owner->sleep_ticks > 0 || owner->stun_ticks > 0 ||
        owner->inventory.held()->kind != ItemKind::HarpoonGun || owner->inventory.held()->flight != handle) {
        release_harpoon(game,handle); return;
    }
    const bool pulling = shot.fixture_open;
    shot.fixture_open = false;
    if (shot.label_b == 1) { reel(game,handle,*owner,pulling); return; }
    if (!tether_clear(game,*owner,shot)) { release_harpoon(game,handle); return; }
    if (shot.timer_b == 0) fly(game,handle);
}

bool valid_harpoon_state(const Entity& entity) {
    if (entity.kind != EntityKind::Projectile || entity.label_a != static_cast<int>(ProjectileKind::Harpoon)) return true;
    return entity.ground_item.kind == ItemKind::HarpoonGun && entity.ground_item.flight.slot < 0 &&
        entity.label_b >= 0 && entity.label_b <= 1 && entity.counter_a >= 0 && entity.counter_a <= 14 &&
        entity.counter_b >= 0 && entity.counter_b <= 100 && entity.counter_c >= 0 && entity.counter_c <= 1 &&
        entity.attack_interval >= 1 && entity.attack_interval <= 14 && entity.timer_b <= 10 &&
        distance({},entity.facing) == 1;
}
