#include "lunch_tin.hpp"
#include "interaction.hpp"

bool thaw_lunch_tin(Game& game, Cell cell) {
    const Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::FrozenLunchTin || tile->prop.broken) return false;
    // LID: The ordinary break path owns the single loot roll and settled debris state.
    return hit_prop(game,cell,prop_max_health(tile->prop),cell);
}
