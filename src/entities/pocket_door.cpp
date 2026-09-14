#include "pocket_door.hpp"
#include "../world/encounter.hpp"
#include "../items/action.hpp"

#include <algorithm>

// SLOTS: entity_a reciprocal endpoint; timer_a shared arrival cooldown prevents ping-pong.
void init_pocket_door(Entity& door) {
    door.sprite = Sprite::PocketThreshold;
    door.health = door.max_health = 1;
    door.hard_blocker = true;
    door.light = {2, 100, {135, 153, 199}};
}

bool pocket_passage_allowed(const Game& game, Cell from, Cell to) {
    for (const Entity& encounter : game.entities) {
        if (encounter.kind != EntityKind::Encounter) continue;
        const auto phase = static_cast<EncounterPhase>(encounter.label_a);
        if (phase == EncounterPhase::Dormant || phase == EncounterPhase::Gathering || phase == EncounterPhase::Released) continue;
        const auto inside = [&encounter](Cell cell) {
            return cell.x >= encounter.point_a.x && cell.y >= encounter.point_a.y &&
                cell.x <= encounter.point_b.x && cell.y <= encounter.point_b.y;
        };
        if (inside(from) != inside(to)) return false;
    }
    return true;
}

const Entity* pocket_partner(const Game& game, const Entity& door) {
    if (door.kind != EntityKind::PocketDoor || !door.fixture_open) return nullptr;
    const Entity* other = get_entity(game, door.entity_a);
    if (!other || other->kind != EntityKind::PocketDoor || !other->fixture_open ||
        get_entity(game, other->entity_a) != &door) return nullptr;
    return other;
}

void enter_pocket_door(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    for (Entity& door : game.entities) {
        if (door.kind != EntityKind::PocketDoor || door.cell != actor.cell || door.timer_a > 0) continue;
        const Entity* other = pocket_partner(game, door);
        if (!other || other->timer_a > 0) continue;
        const Tile* tile = game.stage.at(other->cell);
        const bool clear = tile && walkable(*tile) && entity_at(game, other->cell, true) < 0;
        if (!clear || !pocket_passage_allowed(game, actor.cell, other->cell)) {
            emit_sound(game, SoundId::PocketBlocked, actor.cell);
            return;
        }
        const Cell origin = actor.cell;
        const Handle partner = door.entity_a;
        // ARRIVAL: Close both thresholds briefly before destination hazards can move us again.
        door.timer_a = 12;
        get_entity(game, partner)->timer_a = 12;
        actor.cell = other->cell;
        actor.move_wait = std::max(actor.move_wait, actor.move_interval);
        if (actor.kind == EntityKind::Player) cancel_item_action(actor);
        emit_sound(game, SoundId::PocketTravel, origin);
        emit_sound(game, SoundId::PocketTravel, actor.cell);
        enter_actor_cell(game, slot);
        return;
    }
}
