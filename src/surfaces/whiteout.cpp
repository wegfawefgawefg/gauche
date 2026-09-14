#include "whiteout.hpp"
#include "../entities/hearing.hpp"

#include <algorithm>

bool raise_whiteout(Game& game, Cell center, int radius, int ticks) {
    const Tile* origin = game.stage.at(center);
    if (!origin || !walkable(*origin)) return false;
    // AIR: Walls and closed fixtures constrain this burst before any cells change.
    const auto cells = audible_cells(game,center,std::clamp(radius,0,4));
    bool changed = false;
    for (Cell cell : cells) {
        Tile* tile = game.stage.at(cell);
        if (!tile) continue;
        const auto duration = static_cast<std::uint16_t>(std::clamp(ticks,1,360));
        if (tile->surface.whiteout_ticks >= duration) continue;
        tile->surface.whiteout_ticks = duration;
        changed = true;
    }
    return changed;
}
