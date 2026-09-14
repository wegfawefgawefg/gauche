#include "wolf_call.hpp"
#include "hearing.hpp"
#include "attacks.hpp"
#include "behavior.hpp"

namespace {

bool hostile_prey(const Entity& actor) {
    if (actor.health <= 0 || actor.hard_blocker) return false;
    switch (actor.kind) {
    case EntityKind::RimeSkater:
    case EntityKind::Zombie: case EntityKind::ZombieStack: case EntityKind::Bear:
    case EntityKind::Boar: case EntityKind::Bat: case EntityKind::FrostBat:
    case EntityKind::Ember: case EntityKind::ThornSnail: case EntityKind::LanternMoth:
    case EntityKind::SporeToad: case EntityKind::CrateMimic: case EntityKind::RootTurret:
    case EntityKind::BrambleGuard: case EntityKind::Mosquito: case EntityKind::Owl:
    case EntityKind::Woodpecker: case EntityKind::WaspNest: case EntityKind::Wasp:
    case EntityKind::ForagerGoblin: case EntityKind::CarrionCrow: case EntityKind::BurrowWorm:
        return true;
    default: return false;
    }
}

bool lone_wolf(const Game& game, int slot) {
    const Entity& wolf = game.entities[static_cast<std::size_t>(slot)];
    if (wolf.kind != EntityKind::Wolf || wolf.health <= 0 || wolf.label_a != 0) return false;
    const Entity* leader = get_entity(game, wolf.entity_a);
    if (leader && leader->kind == EntityKind::Wolf && leader->health > 0) return false;
    // PACKS: Do not exploit the interval before nearby wolves choose a leader.
    for (const Entity& other : game.entities) {
        if (&other == &wolf || other.kind != EntityKind::Wolf || other.health <= 0) continue;
        if (distance(wolf.cell, other.cell) <= 7 || other.entity_a == Handle{slot, wolf.generation}) return false;
    }
    return true;
}

Handle choose_prey(const Game& game, Cell from) {
    Handle best;
    int nearest = 9;
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        const int gap = distance(from, actor.cell);
        if (!hostile_prey(actor) || gap >= nearest || !clear_sight(game, from, actor.cell)) continue;
        nearest = gap;
        best = {slot, actor.generation};
    }
    return best;
}

} // namespace

bool call_wolf(Game& game, int caller, Cell direction, int reach) {
    Cell cell = game.entities[static_cast<std::size_t>(caller)].cell;
    const Cell origin = cell;
    for (int step = 1; step <= reach; ++step) {
        cell = cell + direction;
        if (!clear_sight(game, origin, cell)) return false;
        const int slot = entity_at(game, cell, true);
        if (slot < 0) continue;
        if (!lone_wolf(game, slot)) return false;
        Entity& wolf = game.entities[static_cast<std::size_t>(slot)];
        const Handle prey = choose_prey(game, wolf.cell);
        if (prey.slot < 0) return false;
        // ATTENTION: Reuse the prey handle and c memory, preserving home and pack state.
        wolf.entity_b = prey;
        wolf.label_c = WhistleHunt;
        wolf.timer_c = wolf_call_ticks;
        wolf.point_c = cell;
        wolf.sleep_ticks = 0;
        emit_sound(game, SoundId::WolfAnswer, cell);
        return true;
    }
    return false;
}

const Entity* called_wolf_prey(Game& game, Entity& wolf) {
    if (wolf.label_c != WhistleHunt) return nullptr;
    const Entity* prey = get_entity(game, wolf.entity_b);
    if (wolf.timer_c > 0 && prey && hostile_prey(*prey) && distance(wolf.cell, prey->cell) <= 12 &&
        clear_sight(game, wolf.cell, prey->cell)) return prey;
    // LOST: No tracking through walls or chasing a recycled entity slot.
    wolf.timer_c = wolf.label_c = 0;
    wolf.entity_b = {};
    return nullptr;
}
