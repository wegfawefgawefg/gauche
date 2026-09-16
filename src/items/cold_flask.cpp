#include "cold_flask.hpp"
#include "../surfaces/temperature.hpp"
#include "../entities/attacks.hpp"

#include <cstdlib>

namespace {

constexpr RegionalItem flask{"Cold Flask", "Freezes river water. Keep away from heat.",
    Sprite::ColdFlask, {1, 5, 1, 0, 45, PatternEffect::Utility, true, 0, 0, false, false, true},
    ItemAction::Throw, 10, 3, true, 0, 0, 0, 0, 0, SoundId::ColdThrow};

} // namespace

const RegionalItem* cold_flask_item(ItemKind kind) {
    return kind == ItemKind::ColdFlask ? &flask : nullptr;
}

std::vector<Cell> cold_flask_cells(const Game& game, const Item& item, Cell center) {
    const int radius = item_pattern(item).blast_radius;
    std::vector<Cell> exposed;
    // COVER: Capture the cross before quenching or freezing can change its surroundings.
    for (int dy = -radius; dy <= radius; ++dy)
        for (int dx = -radius; dx <= radius; ++dx) {
            if ((dx != 0 && dy != 0) || std::abs(dx) + std::abs(dy) > radius) continue;
            const Cell cell = center + Cell{dx, dy};
            const Tile* tile = game.stage.at(cell);
            if (tile == nullptr || (!walkable(tile->kind) && tile->kind!=TileKind::DeepRiver) ||
                !clear_attack_sight(game, center, cell, false)) continue;
            exposed.push_back(cell);
        }
    return exposed;
}

void cold_flask_impact(Game& game, const Item& item, Cell center) {
    const auto exposed = cold_flask_cells(game, item, center);
    emit_sound(game, SoundId::ColdBurst, center);
    for (Cell cell : exposed) quench_cell(game, cell);
    for (Cell cell : exposed) {
        freeze_water(game, cell, 480);
        const bool warm = warm_cell(game, cell);
        for (Entity& actor : game.entities)
            if (actor.kind != EntityKind::None && actor.cell == cell &&
                (!warm || actor.kind == EntityKind::SteamLeech)) apply_chill(actor, 180);
    }
}
