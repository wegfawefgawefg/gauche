#include "brine.hpp"
#include "../entities/attacks.hpp"
#include "../props/ice_cover.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"

#include <cstdlib>
#include <vector>

namespace {

constexpr RegionalItem flask{"Brine Flask", "Throw salt water. Melts ice; prevents refreezing for 8s. Splash stings wounded actors, including friends.",
    Sprite::BrineFlask, {1,5,1,4,45,PatternEffect::Damage,true},
    ItemAction::Throw,9,3,true,0,0,0,0,0,SoundId::BrineThrow};

} // namespace

std::vector<Cell> brine_cells(const Game& game, const Item& item, Cell center) {
    const int radius = item_pattern(item).blast_radius;
    std::vector<Cell> cells;
    // COVER: An exposed ice block can melt, without exposing another cell in this splash.
    for (int y=-radius;y<=radius;++y)
        for (int x=-radius;x<=radius;++x) {
            if (std::abs(x)+std::abs(y) > radius) continue;
            const Cell cell = center+Cell{x,y};
            const Tile* tile = game.stage.at(cell);
            if (!tile || !walkable(tile->kind) || !clear_attack_sight(game,center,cell,false)) continue;
            cells.push_back(cell);
        }
    return cells;
}

const RegionalItem* brine_item(ItemKind kind) {
    return kind == ItemKind::BrineFlask ? &flask : nullptr;
}

void brine_impact(Game& game, const Item& item, Cell center) {
    const ItemPattern pattern = item_pattern(item);
    const auto cells = brine_cells(game,item,center);
    emit_sound(game,SoundId::BrineSplash,center);
    for (Cell cell : cells) {
        // SALT: Pour first so an ice block's meltwater does not replace the brine.
        if (!pour_surface(game,cell,LiquidKind::Brine,480)) continue;
        melt_ice_cover(game,cell);
        if (thaw_water(game,cell)) emit_sound(game,SoundId::IceThaw,cell);
        quench_cell(game,cell);
        for (int slot=0;slot<max_entities;++slot) {
            const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
            if (actor.cell != cell || actor.health <= 0 || actor.health >= actor.max_health || actor.hard_blocker) continue;
            damage_entity(game,slot,pattern.damage,center,false);
        }
    }
}
