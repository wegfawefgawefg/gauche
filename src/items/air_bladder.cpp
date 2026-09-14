#include "air_bladder.hpp"
#include "../combat/shove.hpp"
#include "../world/floating_items.hpp"
#include "../entities/attacks.hpp"

#include <array>

namespace {

constexpr RegionalItem bladder{"Air Bladder", "Push nearby actors outward, or aim at loose loot in shallow water to float it straight to shore.",
    Sprite::AirBladder, {0, 0, 1, 0, 45, PatternEffect::Utility, false, 0, 0, false, false, true},
    ItemAction::Material, 10, 1, false, 3, 0, 0, 0, 0, SoundId::AirInflate};

} // namespace

const RegionalItem* air_bladder_item(ItemKind kind) {
    return kind == ItemKind::AirBladder ? &bladder : nullptr;
}

int floating_cargo_in_front(const Game& game, const Entity& user, Cell direction) {
    const Cell front = user.cell + direction;
    if (!float_water(game.stage.at_or_border(front))) return -1;
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& item = game.entities[static_cast<std::size_t>(slot)];
        if (item.kind == EntityKind::GroundItem && item.cell == front &&
            item.ground_item.kind != ItemKind::None && item.ground_item.count > 0) return slot;
    }
    return -1;
}

bool use_air_bladder(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const int cargo = floating_cargo_in_front(game, user, direction);
    if (cargo >= 0) return start_item_float(game, cargo, direction);
    const Cell origin = user.cell;
    const int radius = item_pattern(*user.inventory.held()).blast_radius;
    struct Push { Handle actor{}; Cell direction{}, cell{}; };
    std::array<Push, 8> pushes{};
    int count = 0;
    // PULSE: Capture targets before moving anyone. Big pushes outer targets first,
    // so a displaced actor cannot get selected twice in the same inflation.
    for (int reach = radius; reach >= 1; --reach)
        for (Cell side : {Cell{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
            const Cell cell = origin + Cell{side.x * reach, side.y * reach};
            if (!clear_attack_sight(game, origin, cell)) continue;
            const int victim = entity_at(game, cell, true);
            if (victim < 0 || victim == slot) continue;
            const Entity& actor = game.entities[static_cast<std::size_t>(victim)];
            if (actor.health <= 0 || actor.hard_blocker || actor.move_interval <= 0) continue;
            if (count < static_cast<int>(pushes.size())) pushes[static_cast<std::size_t>(count++)] = {{victim, actor.generation}, side, cell};
        }
    bool used = false;
    for (int index = 0; index < count; ++index) {
        const Push push = pushes[static_cast<std::size_t>(index)];
        if (const Entity* actor = get_entity(game, push.actor); actor && actor->health > 0 && actor->cell == push.cell)
            used |= shove_actor(game, push.actor.slot, push.direction, origin);
    }
    return used;
}
