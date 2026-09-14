#include "projectile.hpp"
#include "../items/materials.hpp"
#include "../item_pattern.hpp"
#include "../props/interaction.hpp"

#include <algorithm>

// SLOTS: label_a = kind, label_b = blast radius/arrow piercing, counter_a = range left,
// counter_b = damage, timer_a = fuse/life, timer_b = tile travel. point_a = source,
// entity_a = owner, ground_item = immutable weapon spec, attack_interval = total range.
void init_projectile(Entity& entity) {
    entity.health = entity.max_health = 1;
    entity.sprite = Sprite::Arrow;
    entity.impassable = false;
}

int projectile_step_ticks(const Entity& entity) {
    if (entity.label_a == static_cast<int>(ProjectileKind::Rocket)) return 2;
    return entity.label_a != static_cast<int>(ProjectileKind::Arrow) ? 8 : 3;
}

bool launch_projectile(Game& game, int owner_slot, const Item& item, Cell direction, int reach) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    const bool bomb = item.kind == ItemKind::Bomb;
    const bool rocket = item.kind == ItemKind::RocketLauncher;
    const bool flask = forest_material_item(item.kind) != nullptr;
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, owner.cell));
    if (shot == nullptr) return false;
    const ItemPattern pattern = item_pattern(item);
    shot->label_a = static_cast<int>(bomb ? ProjectileKind::Bomb : rocket ? ProjectileKind::Rocket : flask ? ProjectileKind::Flask : ProjectileKind::Arrow);
    shot->label_b = bomb || rocket ? pattern.blast_radius :
        (pattern.piercing || has_artifact(owner, ArtifactKind::AllPiercing) ? 1 : 0);
    shot->counter_a = std::clamp(reach, 1, pattern.maximum);
    shot->counter_b = pattern.damage;
    shot->attack_interval = shot->counter_a;
    shot->timer_b = projectile_step_ticks(*shot);
    shot->timer_a = bomb ? bomb_fuse_ticks : shot->counter_a * shot->timer_b + 3;
    shot->point_a = owner.cell;
    shot->entity_a = {owner_slot, owner.generation};
    shot->facing = direction;
    shot->ground_item = item;
    shot->ground_item.count = 1;
    shot->sprite = bomb ? Sprite::BombLit : rocket ? Sprite::Rocket : flask ? item_sprite(item) :
        item.kind == ItemKind::Crossbow ? Sprite::Bolt : Sprite::Arrow;
    if (rocket) shot->light = {2, 380, {255, 162, 73}};
    if (bomb) {
        shot->light = {2, 180, {255, 156, 56}};
        emit_sound(game, SoundId::BombFuse, owner.cell);
    }
    if (!flask) emit_sound(game, bomb ? SoundId::BombThrow : rocket ? SoundId::RocketLaunch :
        item.kind == ItemKind::Crossbow ? SoundId::CrossbowShot : SoundId::BowRelease, owner.cell);
    return true;
}

namespace {

void finish_arrow(Game& game, int slot, Cell impact) {
    emit_sound(game, SoundId::ArrowImpact, impact);
    remove_entity(game, {slot, game.entities[static_cast<std::size_t>(slot)].generation});
}

void detonate_projectile(Game& game, int slot, Cell impact) {
    const Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const int radius = shot.label_b, damage = shot.counter_b;
    const Cell attacker = shot.point_a;
    remove_entity(game, {slot, shot.generation});
    blast_area(game, impact, radius, damage, attacker);
}

} // namespace

bool projectile_blocked(const Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind == TileKind::Wall || prop_blocks(tile->prop)) return true;
    for (const Entity& fixture : game.entities)
        if (fixture.cell == cell && fixture.impassable &&
            (fixture.kind == EntityKind::Door || fixture.kind == EntityKind::EncounterGate)) return true;
    return false;
}

Cell bomb_landing(const Game& game, Cell origin, Cell facing, int reach) {
    for (int step = 0; step < reach; ++step) {
        if (projectile_blocked(game, origin + facing)) break;
        origin = origin + facing;
    }
    return origin;
}

void step_projectile(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    const bool bomb = shot.label_a == static_cast<int>(ProjectileKind::Bomb);
    const bool rocket = shot.label_a == static_cast<int>(ProjectileKind::Rocket);
    const bool flask = shot.label_a == static_cast<int>(ProjectileKind::Flask);
    if (flask && (shot.timer_a == 0 || shot.counter_a == 0)) {
        const Item item = shot.ground_item;
        const Cell cell = shot.cell;
        remove_entity(game, {slot, shot.generation});
        material_impact(game, item, cell);
        return;
    }
    if (shot.timer_a == 0) {
        if (bomb || rocket) detonate_projectile(game, slot, shot.cell);
        else finish_arrow(game, slot, shot.cell);
        return;
    }
    if (bomb && shot.timer_a % 30 == 0) emit_sound(game, SoundId::BombFuse, shot.cell);
    if (shot.counter_a == 0 || shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    const bool blocked = projectile_blocked(game, next);
    if (rocket) {
        const int victim = entity_at(game, next, true);
        const bool owner = victim == shot.entity_a.slot && victim >= 0 &&
            game.entities[static_cast<std::size_t>(victim)].generation == shot.entity_a.generation;
        if (blocked || (victim >= 0 && !owner) || shot.counter_a <= 1) {
            detonate_projectile(game, slot, next);
            return;
        }
        shot.cell = next;
        --shot.counter_a;
        shot.timer_b = projectile_step_ticks(shot);
        return;
    }
    if (blocked && (bomb || flask)) {
        shot.counter_a = 0;
        if (bomb) emit_sound(game, SoundId::BombLand, shot.cell);
        return;
    }
    if (!bomb && !flask) {
        hit_prop(game, next, shot.counter_b, shot.cell);
        if (blocked) {
            hit_terrain(game, next, shot.cell, shot.counter_b, shot.ground_item.dig_power);
            finish_arrow(game, slot, next);
            return;
        }
    }
    shot.cell = next;
    --shot.counter_a;
    shot.timer_b = projectile_step_ticks(shot);
    if (bomb || flask) {
        if (bomb && shot.counter_a == 0) emit_sound(game, SoundId::BombLand, shot.cell);
        return;
    }
    for (int victim_slot = 0; victim_slot < max_entities; ++victim_slot) {
        const Entity& victim = game.entities[static_cast<std::size_t>(victim_slot)];
        if (victim_slot == slot || victim.cell != next || !victim.impassable || victim.health <= 0 ||
            (victim_slot == shot.entity_a.slot && victim.generation == shot.entity_a.generation)) continue;
        damage_entity(game, victim_slot, shot.counter_b, next - shot.facing);
        emit_sound(game, SoundId::ArrowImpact, next);
        if (shot.label_b == 0) { remove_entity(game, {slot, shot.generation}); return; }
    }
    if (shot.counter_a == 0) finish_arrow(game, slot, next);
}
