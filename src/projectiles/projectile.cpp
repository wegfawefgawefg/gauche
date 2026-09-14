#include "projectile.hpp"
#include "../items/muffling.hpp"
#include "frost.hpp"
#include "ice_brick.hpp"
#include "snowball.hpp"
#include "prism.hpp"
#include "hook.hpp"
#include "fishing.hpp"
#include "root_drill.hpp"
#include "swap.hpp"
#include "recoverable.hpp"
#include "net.hpp"
#include "thunder.hpp"
#include "../items/noisemakers.hpp"
#include "../items/mixtures.hpp"
#include "../items/materials.hpp"
#include "../item_pattern.hpp"
#include "../combat/parry.hpp"
#include "../props/interaction.hpp"

#include <algorithm>

// SLOTS: label_a = kind, label_b = blast radius/arrow piercing, counter_a = range left,
// counter_b = damage, timer_a = fuse/life, timer_b = tile travel. point_a = source,
// entity_a = current owner, ground_item = weapon spec, attack_interval = total range.
// timer_c = hard lifetime for reflecting arrow/rocket legs; never refreshed on a parry.
void init_projectile(Entity& entity) {
    entity.health = entity.max_health = 1;
    entity.sprite = Sprite::Arrow;
    entity.impassable = false;
}

int projectile_step_ticks(const Entity& entity) {
    if (entity.label_a == static_cast<int>(ProjectileKind::FishingHook)) return fishing_beat;
    if (entity.label_a == static_cast<int>(ProjectileKind::Snowball)) return 5;
    if (entity.label_a == static_cast<int>(ProjectileKind::FrostPuff)) return frost_puff_step_ticks;
    if (entity.label_a == static_cast<int>(ProjectileKind::ThunderAcorn)) return 4;
    if (entity.label_a == static_cast<int>(ProjectileKind::Net)) return 4;
    if (entity.label_a == static_cast<int>(ProjectileKind::Hook) || entity.label_a == static_cast<int>(ProjectileKind::Swap)) return 4;
    if (entity.label_a == static_cast<int>(ProjectileKind::Drill)) return 6;
    if (entity.label_a == static_cast<int>(ProjectileKind::Rock)) return 4;
    if (entity.label_a == static_cast<int>(ProjectileKind::Boomerang)) return 3;
    if (entity.label_a == static_cast<int>(ProjectileKind::Rocket)) return 2;
    return entity.label_a != static_cast<int>(ProjectileKind::Arrow) ? 8 : 3;
}

bool launch_projectile(Game& game, int owner_slot, const Item& item, Cell direction, int reach) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    const bool bomb = item.kind == ItemKind::Bomb;
    const bool rocket = item.kind == ItemKind::RocketLauncher;
    const bool flask = item.kind == ItemKind::ColdFlask || forest_material_item(item.kind) != nullptr;
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
    if (!bomb && !flask) shot->timer_c = shot->timer_a * 4;
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
    if (!flask) emit_weapon_sound(game, item, bomb ? SoundId::BombThrow : rocket ? SoundId::RocketLaunch :
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
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::FishingHook)) { step_fishing_hook(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::PrismBomb)) { step_prism_bomb(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::Snowball)) { step_snowball(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::IceBrick)) { step_ice_brick_flight(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::FrostPuff)) { step_frost_puff(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::ThunderAcorn)) { step_thunder_acorn(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::Mixture)) { step_mixture(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::Firecracker)) { step_firecracker(game, slot); return; }
    if (game.entities[static_cast<std::size_t>(slot)].label_a == static_cast<int>(ProjectileKind::Net)) { step_net(game, slot); return; }
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (shot.label_a == static_cast<int>(ProjectileKind::Hook)) { step_hook(game, slot); return; }
    if (shot.label_a == static_cast<int>(ProjectileKind::Drill)) { step_root_drill(game, slot); return; }
    if (shot.label_a == static_cast<int>(ProjectileKind::Swap)) { step_swap_seed(game, slot); return; }
    if (shot.label_a == static_cast<int>(ProjectileKind::Rock) || shot.label_a == static_cast<int>(ProjectileKind::Boomerang)) {
        step_recoverable(game, slot); return;
    }
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
        if (!blocked && victim >= 0 && !owner && parry_ranged_hit(game, victim, shot.facing)) {
            reflect_projectile(shot, game.entities[static_cast<std::size_t>(victim)], victim);
            return;
        }
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
        if (parry_ranged_hit(game, victim_slot, shot.facing)) {
            reflect_projectile(shot, victim, victim_slot);
            return;
        }
        damage_entity(game, victim_slot, shot.counter_b, next - shot.facing);
        emit_sound(game, SoundId::ArrowImpact, next);
        if (shot.label_b == 0) { remove_entity(game, {slot, shot.generation}); return; }
    }
    if (shot.counter_a == 0) finish_arrow(game, slot, next);
}
