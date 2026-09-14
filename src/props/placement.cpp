#include "interaction.hpp"

#include <array>

void scatter_forest_props(Game& game) {
    if (game.run.floor > 4) return;
    constexpr std::array<Cell, 4> directions{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
    for (int y = 2; y < game.stage.height - 2; ++y) {
        for (int x = 2; x < game.stage.width - 2; ++x) {
            const Cell cell{x, y};
            const Tile& tile = *game.stage.at(cell);
            if (!walkable(tile.kind) || tile.kind == TileKind::Lava ||
                distance(cell, game.run.spawn) < 3 || distance(cell, game.run.exit) < 3 ||
                entity_at(game, cell, false) >= 0) continue;
            int walls = 0;
            for (Cell direction : directions)
                if (!walkable(game.stage.at_or_border(cell + direction).kind)) ++walls;
            const auto roll = random_u32(game);
            // CLUSTERS: Growth hugs room edges; the central route remains mostly calm.
            if (roll % 100 >= (walls > 0 ? 23U : 3U)) continue;
            constexpr PropKind plants[]{PropKind::Leaves, PropKind::Twigs, PropKind::Fern,
                PropKind::TallGrass, PropKind::Puffball, PropKind::Nest};
            PropKind kind = plants[(roll >> 8) % 6];
            if (walls == 1 && (roll >> 16) % 4 == 0) {
                // PASSAGE: Leave gaps between blocking decorations and space around objectives.
                bool open = true;
                for (int dy = -1; dy <= 1; ++dy)
                    for (int dx = -1; dx <= 1; ++dx)
                        if (prop_blocks(game.stage.at_or_border(cell + Cell{dx, dy}).prop))
                            open = false;
                for (const Entity& fixture : game.entities)
                    if ((fixture.kind == EntityKind::Door || fixture.kind == EntityKind::Key ||
                         fixture.kind == EntityKind::Switch || fixture.kind == EntityKind::Exit) &&
                        distance(cell, fixture.cell) < 3) open = false;
                if (open) {
                    constexpr PropKind containers[]{PropKind::RottenLog, PropKind::Crate,
                                                     PropKind::ClayPot};
                    kind = containers[(roll >> 20) % 3];
                }
            }
            place_prop(game.stage, cell, kind, static_cast<std::uint8_t>(roll >> 24));
        }
    }
}
