#include "rail_points.hpp"

bool live_rail_points(const Prop& prop) {
    return prop.kind==PropKind::RailPoints && !prop.broken && prop.hp>0;
}
Cell rail_exit(const Stage& stage,Cell cell,Cell incoming) {
    const Tile* tile=stage.at(cell);
    return tile && tile->kind==TileKind::Rail && live_rail_points(tile->prop) ?
        rail_directions[tile->prop.variant&3U] : incoming;
}
bool turn_rail_points(Game& game,Cell cell) {
    Tile* tile=game.stage.at(cell);
    if (!tile || tile->kind!=TileKind::Rail || !live_rail_points(tile->prop)) return false;
    for (const Entity& e:game.entities)
        if (e.kind==EntityKind::RailCart && e.health>0 && e.cell==cell) return false;
    // Cycle connected track ends, not arbitrary directions. Blocked exits stay
    // selectable: a cart must stop at obstructions rather than reroute itself.
    int exits=0;
    for (Cell dir:rail_directions)
        if (game.stage.at_or_border(cell+dir).kind==TileKind::Rail) ++exits;
    if (exits<2) return false;
    for (unsigned offset=1;offset<=4;++offset) {
        const unsigned next=(tile->prop.variant+offset)&3U;
        if (game.stage.at_or_border(cell+rail_directions[next]).kind!=TileKind::Rail) continue;
        if (next==tile->prop.variant) return false;
        tile->prop.variant=static_cast<std::uint8_t>(next);return true;
    }
    return false;
}
