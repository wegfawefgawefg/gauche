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

bool walkable(TileKind kind) {
    return kind == TileKind::Empty || kind == TileKind::Grass ||
           kind == TileKind::Ruin || kind == TileKind::Rail ||
           kind == TileKind::Lava || kind == TileKind::Ice;
}

bool buildable(TileKind kind) {
    return kind == TileKind::Empty || kind == TileKind::Grass;
}

bool damage_tile(Stage& stage, Cell cell, int damage) {
    Tile* tile = stage.at(cell);
    if (tile == nullptr || tile->kind != TileKind::Wall || damage <= 0) return false;
    tile->hp = static_cast<std::uint8_t>(std::max(0, static_cast<int>(tile->hp) - damage));
    if (tile->hp == 0) tile->kind = TileKind::Ruin;
    return true;
}
