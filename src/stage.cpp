#include "game.hpp"

#include <algorithm>
#include <cstdlib>

int distance(Cell a, Cell b) { return std::abs(a.x - b.x) + std::abs(a.y - b.y); }

bool Stage::in_bounds(Cell cell) const {
    return cell.x >= 0 && cell.y >= 0 && cell.x < width && cell.y < height;
}

Tile* Stage::at(Cell cell) {
    if (!in_bounds(cell)) return nullptr;
    return &tiles[static_cast<std::size_t>(cell.y * width + cell.x)];
}

const Tile* Stage::at(Cell cell) const {
    if (!in_bounds(cell)) return nullptr;
    return &tiles[static_cast<std::size_t>(cell.y * width + cell.x)];
}

const Tile& Stage::at_or_border(Cell cell) const {
    static constexpr Tile border{TileKind::Wall, 100, 0, 100, BreakRule::Unbreakable, 0};
    const Tile* tile = at(cell);
    return tile == nullptr ? border : *tile;
}

bool walkable(TileKind kind) {
    return kind == TileKind::Empty || kind == TileKind::Grass ||
           kind == TileKind::Ruin || kind == TileKind::Rail ||
           kind == TileKind::Lava || kind == TileKind::Ice;
}

bool buildable(TileKind kind) {
    return kind == TileKind::Empty || kind == TileKind::Grass;
}

bool walkable(const Tile& tile) {
    return walkable(tile.kind) && !prop_blocks(tile.prop);
}

bool damage_tile(Stage& stage, Cell cell, int damage, int dig_power, TileImpact impact) {
    Tile* tile = stage.at(cell);
    if (tile == nullptr) return false;
    // RAIL: The conductor lays track through all in-bounds material before the train arrives.
    if (impact == TileImpact::Train) {
        const Prop broken_prop = tile->prop;
        *tile = {TileKind::Rail, 0, 0};
        tile->prop = broken_prop;
        return true;
    }
    if (tile->kind != TileKind::Wall || tile->hp == 0 || damage <= 0 ||
        tile->break_rule == BreakRule::Unbreakable) return false;
    if (tile->break_rule == BreakRule::DigRequired &&
        dig_power < tile->required_dig_power) return false;
    tile->hp = static_cast<std::uint16_t>(std::max(0, static_cast<int>(tile->hp) - damage));
    if (tile->hp == 0) {
        tile->kind = TileKind::Ruin;
        tile->break_rule = BreakRule::Unbreakable;
    }
    return true;
}

bool hit_terrain(Game& game, Cell cell, Cell source, int damage, int dig_power,
                 TileImpact impact) {
    const Tile* tile = game.stage.at(cell);
    if (tile == nullptr || tile->kind != TileKind::Wall) return false;
    const int previous = tile->hp;
    const bool hit = damage_tile(game.stage, cell, damage, dig_power, impact);
    if (game.impact_count < static_cast<int>(game.impacts.size()))
        game.impacts[static_cast<std::size_t>(game.impact_count++)] =
            {cell, source, game.run.phase == RunPhase::Arena ? Sprite::Wall :
                Sprite::ForestWall, hit ? previous - tile->hp : 0, hit && tile->hp == 0};
    emit_sound(game, hit ? (tile->hp == 0 ? SoundId::BoxBreak : SoundId::HitBlock1) :
               SoundId::SturdyBlockBouncedOn, cell);
    return hit;
}
