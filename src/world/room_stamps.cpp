#include "floor_internal.hpp"

void carve_room_area(Stage& stage, int left, int top, int right, int bottom, TileKind kind) {
    for (int y = top; y <= bottom; ++y) {
        for (int x = left; x <= right; ++x) {
            Tile* tile = stage.at({x, y});
            if (tile != nullptr) *tile = {kind, 0, 0};
        }
    }
}

void stamp_room(Game& game, int column, int row, bool main_route) {
    const int x = column * room_width;
    const int y = row * room_height;
    const int world = (game.run.floor - 1) / 4;
    const int local_floor = (game.run.floor - 1) % 4;
    carve_room_area(game.stage, x + 1, y + 1, x + 10, y + 8, TileKind::Empty);
    const int variant = static_cast<int>(random_u32(game) % 5);
    const TileKind growth = world == 0 ? TileKind::Grass :
                            (world == 1 ? TileKind::Lava : TileKind::Ice);
    if (variant == 0) {
        carve_room_area(game.stage, x + 2, y + 2, x + 3, y + 3, growth);
        carve_room_area(game.stage, x + 8, y + 6, x + 9, y + 7, growth);
    } else if (variant == 1) {
        carve_room_area(game.stage, x + 2, y + 6, x + 3, y + 7, TileKind::Ruin);
        carve_room_area(game.stage, x + 8, y + 2, x + 9, y + 3, TileKind::Ruin);
    } else if (variant == 2) {
        carve_room_area(game.stage, x + 2, y + 2, x + 2, y + 7, growth);
        carve_room_area(game.stage, x + 9, y + 2, x + 9, y + 7, growth);
    } else if (variant == 3) {
        carve_room_area(game.stage, x + 3, y + 3, x + 8, y + 3, growth);
        carve_room_area(game.stage, x + 3, y + 6, x + 8, y + 6, growth);
    } else {
        carve_room_area(game.stage, x + 4, y + 3, x + 7, y + 6, growth);
    }
    if (!main_route && (variant == 2 || local_floor == 2)) {
        *game.stage.at({x + 3, y + 5}) = {TileKind::Wall, 100, 0};
        *game.stage.at({x + 8, y + 4}) = {TileKind::Wall, 100, 0};
    }
    if (main_route && local_floor == 3 && column > 0 && column < 5) {
        *game.stage.at({x + 5, y + 3}) = {TileKind::Wall, 100, 0};
        *game.stage.at({x + 6, y + 6}) = {TileKind::Wall, 100, 0};
    }
}

void connect_branch(Game& game, int column, int row) {
    stamp_room(game, column, row, false);
    const int center = column * room_width + 6;
    if (row == 0)
        carve_room_area(game.stage, center - 1, 8, center + 1, 11, TileKind::Empty);
    else
        carve_room_area(game.stage, center - 1, 18, center + 1, 21, TileKind::Empty);
}
