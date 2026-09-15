#include "wind.hpp"
#include "interaction.hpp"
#include "../entities/attacks.hpp"
#include "../props/candle.hpp"
#include "../props/stove.hpp"
#include <algorithm>

namespace {
bool open_air(const Game& game,Cell cell) {
    const Tile* tile=game.stage.at(cell);
    if (!tile || !walkable(tile->kind) || (prop_blocks(tile->prop) && !prop_shoot_through(tile->prop))) return false;
    const int blocker=entity_at(game,cell,true);
    return blocker<0 || !game.entities[static_cast<std::size_t>(blocker)].hard_blocker;
}
bool flame(const Game& game,Cell cell) {
    const Tile& tile=game.stage.at_or_border(cell);
    if (tile.surface.fire_ticks || prop_has_flame(tile.prop)) return true;
    for (const Entity& actor:game.entities) {
        if (actor.kind==EntityKind::None || actor.cell!=cell) continue;
        if (actor.kind==EntityKind::Campfire && actor.fire_tramples<5) return true;
        if (actor.kind==EntityKind::GroundItem && actor.ground_item.flame_ticks>0) return true;
    }
    return false;
}
}
std::vector<Cell> gust_cells(const Game& game,Cell source,Cell direction,ItemPattern pattern) {
    std::vector<Cell> cells;
    if (distance({},direction)!=1) return cells;
    // Far-to-near order makes each captured cloud move exactly one cell.
    for (int reach=pattern.maximum;reach>=pattern.minimum;--reach)
        for (int lane=-pattern_half_width(pattern,reach);lane<=pattern_half_width(pattern,reach);++lane) {
            const Cell cell=source+Cell{direction.x*reach-direction.y*lane,direction.y*reach+direction.x*lane};
            if (open_air(game,cell) && clear_sight(game,source,cell,false)) cells.push_back(cell);
        }
    return cells;
}
void blow_surface_air(Game& game,const std::vector<Cell>& cells,Cell direction) {
    struct Air {Cell cell;std::uint16_t smoke,sleep,whiteout;bool burning;};
    std::vector<Air> captured;captured.reserve(cells.size());
    for (Cell cell:cells) {
        const Surface& s=game.stage.at_or_border(cell).surface;
        captured.push_back({cell,s.smoke_ticks,s.sleep_ticks,s.whiteout_ticks,flame(game,cell)});
    }
    for (const Air& air:captured) {
        const Cell destination=air.cell+direction;
        if (!open_air(game,destination)) continue;
        Surface& from=game.stage.at(air.cell)->surface;
        Surface& to=game.stage.at(destination)->surface;
        from.smoke_ticks=from.sleep_ticks=from.whiteout_ticks=0;
        to.smoke_ticks=std::max(to.smoke_ticks,air.smoke);
        to.sleep_ticks=std::max(to.sleep_ticks,air.sleep);
        to.whiteout_ticks=std::max(to.whiteout_ticks,air.whiteout);
    }
    // A puff advances existing flame one cell into actual fuel. Newly lit cells
    // cannot chain through the entire cone, and old flame gains no free lifetime.
    for (const Air& air:captured) if (air.burning) {
        const Cell target=air.cell+direction;
        const int blocker=entity_at(game,target,true);
        if (blocker>=0 && game.entities[static_cast<std::size_t>(blocker)].hard_blocker) continue;
        ignite_surface(game,target);
    }
}
