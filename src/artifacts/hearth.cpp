#include "hearth.hpp"
#include "../entities/attacks.hpp"

#include <algorithm>

bool hearth_meal(ItemKind kind) {
    return kind == ItemKind::LunchTin || kind == ItemKind::CookedMeat || kind == ItemKind::FriedEgg ||
        kind == ItemKind::SmokedFish || kind == ItemKind::HotBroth;
}

namespace {
bool receives_meal(const Game& game, const Entity& source, const Entity* target) {
    return target != nullptr && target->health > 0 && target->health < target->max_health &&
        distance(source.cell, target->cell) <= hearth_meal_radius &&
        clear_sight(game, source.cell, target->cell, false);
}
}

bool hearth_meal_needed(const Game& game, const Entity& source, ItemKind kind) {
    if (source.health <= 0 || !has_artifact(source, ArtifactKind::Hearth) || !hearth_meal(kind)) return false;
    for (Handle handle : controlled_entities(game))
        if (receives_meal(game, source, get_entity(game, handle))) return true;
    return false;
}

void share_hearth_meal(Game& game, const Entity& source, ItemKind kind) {
    if (source.health <= 0 || !has_artifact(source, ArtifactKind::Hearth) || !hearth_meal(kind)) return;
    // ONE PORTION: Called only after successful food use, before consuming the item.
    // Recipient artifacts never retrigger this meal; multiple owners cannot multiply it.
    for (Handle handle : controlled_entities(game)) {
        Entity* target = get_entity(game, handle);
        if (receives_meal(game, source, target))
            target->health = std::min(target->max_health, target->health + hearth_meal_heal);
    }
}
