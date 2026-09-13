#include "../game.hpp"

namespace {

std::uint32_t noise_at(int x, int y, std::uint64_t seed) {
    std::uint64_t value = static_cast<std::uint64_t>(x) * std::uint64_t{0x9E3779B185EBCA87ULL} ^
                          static_cast<std::uint64_t>(y) * std::uint64_t{0xC2B2AE3D27D4EB4FULL} ^ seed;
    value ^= value >> 33;
    value *= 0xff51afd7ed558ccdULL;
    value ^= value >> 33;
    return static_cast<std::uint32_t>(value >> 32);
}

Cell open_cell(Game& game, Cell center, int radius) {
    for (int attempt = 0; attempt < 4096; ++attempt) {
        const int span = radius * 2 + 1;
        const int x = center.x + static_cast<int>(random_u32(game) % static_cast<std::uint32_t>(span)) - radius;
        const int y = center.y + static_cast<int>(random_u32(game) % static_cast<std::uint32_t>(span)) - radius;
        const Cell cell{x, y};
        const Tile* tile = game.stage.at(cell);
        if (tile != nullptr && walkable(tile->kind) && entity_at(game, cell, true) < 0) return cell;
    }
    return center;
}

} // namespace

void start_test_arena(Game& game, std::uint64_t seed) {
    game = {};
    game.rng = seed == 0 ? 1 : seed;
    game.run.phase = RunPhase::Arena;
    game.run.online[0] = true;
    game.stage.width = 64;
    game.stage.height = 64;
    game.stage.tiles.resize(64U * 64U);
    for (int y = 0; y < game.stage.height; ++y) {
        for (int x = 0; x < game.stage.width; ++x) {
            Tile& tile = *game.stage.at({x, y});
            const std::uint32_t n = noise_at(x / 5, y / 5, seed);
            const int value = static_cast<int>(n % 100U);
            if (value > 76) tile.kind = TileKind::Grass;
            if (value < 4) {
                tile.kind = TileKind::Water;
                tile.water_phase = static_cast<std::uint8_t>(random_u32(game) % 2);
            }
        }
    }
    const Cell center{32, 32};
    game.players[0] = spawn_entity(game, EntityKind::Player, open_cell(game, center, 5));
    if (Entity* player = get_entity(game, game.players[0])) player->owner = 0;
    for (int index = 0; index < 32; ++index) {
        spawn_entity(game, EntityKind::Zombie, open_cell(game, center, 31));
        spawn_entity(game, EntityKind::Chicken, open_cell(game, center, 31));
    }
    game.started = true;
}
