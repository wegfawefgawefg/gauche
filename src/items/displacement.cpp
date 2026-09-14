#include "displacement.hpp"
#include "../combat/shove.hpp"
#include "../entities/attacks.hpp"

#include <algorithm>
#include <vector>

namespace {

constexpr RegionalItem horn{"Hunting Horn", "Push each target 1 cell. Walls crush; friends too. Wakes sleepers in the cone.",
    Sprite::HuntingHorn, {1, 3, 0, 0, 100, PatternEffect::Utility, false, 2, 0, false, true},
    ItemAction::Material, 22, 1, false, 8, 0, 0, 0, 0, SoundId::HornBlast};
constexpr RegionalItem hook{"Rope Hook", "Reel a target or yourself toward an anchor by up to 3 cells. Step sideways to break free.",
    Sprite::RopeHook, {1, 6, 0, 0, 75, PatternEffect::Utility, true},
    ItemAction::Material, 25, 1, false, 24, 0, 0, 0, 0, SoundId::HookCast};

} // namespace

const RegionalItem* forest_displacement_item(ItemKind kind) {
    switch (kind) {
    case ItemKind::HuntingHorn: return &horn;
    case ItemKind::RopeHook: return &hook;
    default: return nullptr;
    }
}

bool movable_by_tool(const Entity& actor) {
    return !actor.hard_blocker && (actor.kind == EntityKind::GroundItem ||
        (actor.health > 0 && actor.move_interval > 0));
}

bool blow_hunting_horn(Game& game, int slot, Cell direction) {
    const Entity& user = game.entities[static_cast<std::size_t>(slot)];
    const Cell origin = user.cell;
    const ItemPattern pattern = item_pattern(*user.inventory.held());
    struct Target { Handle handle; int reach; };
    std::vector<Target> targets;
    // COMMIT: Capture once, far to near. A shove cannot put the same actor in a second hit cell.
    for (int index = 0; index < max_entities; ++index) {
        const Entity& actor = game.entities[static_cast<std::size_t>(index)];
        if (index == slot || actor.kind == EntityKind::None || !movable_by_tool(actor)) continue;
        const Cell offset = actor.cell - origin;
        const int reach = offset.x * direction.x + offset.y * direction.y;
        const int lane = offset.y * direction.x - offset.x * direction.y;
        if (reach < pattern.minimum || reach > pattern.maximum ||
            std::abs(lane) > pattern_half_width(pattern, reach) ||
            !clear_sight(game, origin, actor.cell, false)) continue;
        targets.push_back({{index, actor.generation}, reach});
    }
    std::stable_sort(targets.begin(), targets.end(), [](Target a, Target b) { return a.reach > b.reach; });
    for (const Target& target : targets) {
        Entity* actor = get_entity(game, target.handle);
        if (actor == nullptr) continue;
        actor->sleep_ticks = 0;
        shove_actor(game, target.handle.slot, direction, origin);
    }
    if (game.sweep_count < static_cast<int>(game.sweeps.size()))
        game.sweeps[static_cast<std::size_t>(game.sweep_count++)] =
            {origin, direction, pattern.maximum, pattern.half_width, true};
    return true;
}
