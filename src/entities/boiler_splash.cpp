#include "boiler_tank.hpp"
#include "boiler_feed.hpp"
#include "attacks.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"
#include "../props/ice_cover.hpp"

#include <algorithm>

std::vector<Cell> boiler_splash_cells(const Game& game, Cell source, Cell direction, bool rupture) {
    std::vector<Cell> cells;
    const Cell sides[]{direction,{-direction.x,-direction.y},{-direction.y,direction.x},{direction.y,-direction.x}};
    if (rupture) cells.push_back(source);
    for (int lane=0;lane<(rupture ? 4 : 1);++lane)
        for (int reach=1;reach<=(rupture ? 3 : 4);++reach) {
            const Cell cell = source+Cell{sides[lane].x*reach,sides[lane].y*reach};
            const Tile* tile = game.stage.at(cell);
            if (!tile || !walkable(tile->kind) || !clear_attack_sight(game,source,cell,false)) break;
            cells.push_back(cell);
        }
    return cells;
}

void boiler_splash(Game& game, int slot, bool rupture) {
    const Entity& tank = game.entities[static_cast<std::size_t>(slot)];
    if (!consume_boiler_water(game,tank,rupture)) return;
    const Cell source = tank.cell;
    const bool hot = tank.counter_a >= 25;
    const int damage = hot ? (rupture ? 24 : 18) : 0;
    // COMMIT: Melting cover or a reflected fatal hit cannot extend this spray's footprint.
    const auto cells = boiler_splash_cells(game,source,rupture ? Cell{1,0} : tank.point_b,rupture);
    for (Cell cell : cells) {
        if (damage > 0)
            for (int victim=0;victim<max_entities;++victim) {
                const Entity& actor = game.entities[static_cast<std::size_t>(victim)];
                if (victim != slot && actor.kind != EntityKind::None && actor.health > 0 && actor.cell == cell)
                    damage_entity(game,victim,damage,source);
            }
        if (hot) { melt_ice_cover(game,cell); thaw_water(game,cell); }
        pour_surface(game,cell,LiquidKind::Water,600);
        quench_cell(game,cell);
        if (hot) {
            Tile* tile = game.stage.at(cell);
            tile->surface.warmth_ticks = std::max<std::uint16_t>(tile->surface.warmth_ticks,60);
        }
        emit_sound(game,hot ? SoundId::BoilerScald : SoundId::KettleSplash,cell);
    }
}

void rupture_boiler(Game& game, int slot) {
    Entity& tank = game.entities[static_cast<std::size_t>(slot)];
    if (tank.kind != EntityKind::BoilerTank || tank.label_b != 0) return;
    // CHAINS: Mark the vessel spent before another rupture can damage it recursively.
    tank.label_b = 1;
    boiler_splash(game,slot,true);
    tank.counter_a = tank.counter_b = 0;
}
