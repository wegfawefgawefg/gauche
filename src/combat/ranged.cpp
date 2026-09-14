#include "ranged.hpp"
#include "parry.hpp"
#include "../item_pattern.hpp"
#include "../props/interaction.hpp"

namespace {

void trace_event(Game& game, Cell source, Cell end, bool impact, bool muzzle, ItemKind kind) {
    if (game.shot_count >= static_cast<int>(game.shots.size())) return;
    const bool casing = muzzle && (kind == ItemKind::Pistol || kind == ItemKind::SMG || kind == ItemKind::Shotgun);
    game.shots[static_cast<std::size_t>(game.shot_count++)] = {source, end, impact, muzzle, casing};
}

} // namespace

void fire_bullet(Game& game, int owner_slot, Cell source, Cell direction,
                 const Item& weapon, bool muzzle) {
    const Entity& owner = game.entities[static_cast<std::size_t>(owner_slot)];
    const ItemPattern pattern = item_pattern(weapon);
    const bool piercing = pattern.piercing || has_artifact(owner, ArtifactKind::AllPiercing);
    const bool burning = owner.kind == EntityKind::Ember;
    const ItemKind kind = burning ? ItemKind::None : weapon.kind;
    Handle shooter{owner_slot, owner.generation};
    Cell cell = source;
    Cell attacker = owner.cell;
    bool impact = false;
    int remaining = pattern.maximum;
    // BUDGET: Reflections get a fresh leg, with at most four original ranges in total.
    for (int budget = pattern.maximum * 4; budget > 0 && remaining > 0; --budget) {
        const Cell next = cell + direction;
        const Tile* tile = game.stage.at(next);
        if (tile == nullptr) break;
        cell = next;
        --remaining;
        const bool prop = prop_blocks(tile->prop);
        hit_prop(game, cell, pattern.damage, attacker);
        if (prop && !piercing) { impact = true; break; }
        if (tile->kind == TileKind::Wall) {
            hit_terrain(game, cell, attacker, pattern.damage, weapon.dig_power);
            impact = true;
            break;
        }
        const int target = entity_at(game, cell, true);
        if (target < 0) continue;
        const Entity& actor = game.entities[static_cast<std::size_t>(target)];
        if (shooter == Handle{target, actor.generation}) continue;
        if (parry_ranged_hit(game, target, direction)) {
            trace_event(game, source, cell, true, muzzle, kind);
            source = attacker = cell;
            shooter = {target, actor.generation};
            direction = {-direction.x, -direction.y};
            remaining = pattern.maximum;
            muzzle = false;
            continue;
        }
        const int health = actor.health;
        damage_entity(game, target, pattern.damage, attacker);
        if (burning && actor.health < health)
            game.entities[static_cast<std::size_t>(target)].burn_ticks = 120;
        if (!piercing || actor.hard_blocker) { impact = true; break; }
    }
    if (cell != source || muzzle) trace_event(game, source, cell, impact, muzzle, kind);
}
