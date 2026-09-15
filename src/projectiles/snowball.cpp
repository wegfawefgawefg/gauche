#include "snowball.hpp"
#include "projectile.hpp"
#include "../item_pattern.hpp"
#include "../combat/parry.hpp"
#include "../props/interaction.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"

namespace {

void splat(Game& game, int slot, Cell cell) {
    remove_entity(game, {slot, game.entities[static_cast<std::size_t>(slot)].generation});
    quench_cell(game, cell);
    pour_surface(game, cell, LiquidKind::Water, 120);
    emit_sound(game, SoundId::SnowSplat, cell);
}

} // namespace

// SLOTS: counter_a range; counter_b damage; timer_a life, timer_b travel, timer_c hard deadline.
// entity_a attacker; ground_item keeps attributes; label_b is the All Piercing flag.
bool launch_snowball(Game& game, int owner, const Item& item, Cell direction) {
    const Entity& actor = game.entities[static_cast<std::size_t>(owner)];
    Entity* shot = get_entity(game, spawn_entity(game, EntityKind::Projectile, actor.cell));
    if (!shot) return false;
    const ItemPattern pattern = item_pattern(item);
    shot->label_a = static_cast<int>(ProjectileKind::Snowball);
    shot->label_b = has_artifact(actor, ArtifactKind::AllPiercing) ? 1 : 0;
    shot->counter_a = shot->attack_interval = pattern.maximum;
    shot->counter_b = pattern.damage;
    shot->entity_a = {owner, actor.generation};
    shot->point_a = actor.cell;
    shot->facing = direction;
    shot->ground_item = item;
    shot->ground_item.count = 1;
    shot->sprite = Sprite::Snowball;
    shot->timer_b = 5;
    shot->timer_a = pattern.maximum * 5 + 1;
    shot->timer_c = shot->timer_a * 4;
    return true;
}

void step_snowball(Game& game, int slot) {
    Entity& shot = game.entities[static_cast<std::size_t>(slot)];
    if (shot.timer_a == 0 || shot.counter_a == 0) { splat(game, slot, shot.cell); return; }
    if (shot.timer_b > 0) return;
    const Cell next = shot.cell + shot.facing;
    // COVER: Even a prop broken by this hit stops the original snowball.
    if (projectile_blocked(game, next)) {
        hit_prop(game, next, shot.counter_b, shot.cell);
        splat(game, slot, shot.cell);
        return;
    }
    shot.cell = next;
    --shot.counter_a;
    shot.timer_b = 5;
    for (int victim = 0; victim < max_entities; ++victim) {
        Entity& target = game.entities[static_cast<std::size_t>(victim)];
        if (!target.impassable || target.health <= 0 || target.cell != next ||
            Handle{victim, target.generation} == shot.entity_a) continue;
        if (parry_ranged_hit(game, victim, shot.facing)) { reflect_projectile(shot, target, victim); return; }
        damage_entity(game, victim, shot.counter_b, next - shot.facing, true, shot.entity_a);
        if (shot.label_b == 0) { splat(game, slot, next); return; }
        quench_cell(game, next);
        pour_surface(game, next, LiquidKind::Water, 120);
        emit_sound(game, SoundId::SnowSplat, next);
    }
    // HEAT: Landing in a flame cools that cell; there is no recovered ball to ignite.
    if (shot.counter_a == 0 || hot_cell(game, next)) splat(game, slot, next);
}
