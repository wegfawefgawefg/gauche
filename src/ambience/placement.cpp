#include "system.hpp"

#include <algorithm>

void place_ambience(AmbientAudio& audio, const Game& game, Cell listener) {
    audio.sources.clear();
    if (game.run.floor < 1 || game.run.floor > 4) return;
    const auto add = [&](AmbientCue cue, Cell cell, bool global = false,
                         Handle owner = {}, PropKind prop = PropKind::None) {
        if (audio.sources.size() >= 128) return;
        int siblings = 0;
        for (const AmbientSource& source : audio.sources) {
            if (source.cue != cue) continue;
            if (distance(source.cell, cell) < 12 && !global) return;
            ++siblings;
        }
        if (siblings >= 4) return;
        const AmbientSpec& spec = ambient_specs[static_cast<std::size_t>(cue)];
        audio.sources.push_back({cue, cell, owner, prop, spec.cooldown * .5F,
            global, static_cast<float>(distance(cell, listener)) <= spec.trigger_radius, false});
    };
    // LEVEL: These schedules are local; creating ambience never advances game.rng.
    add(AmbientCue::ForestWind, game.run.spawn, true);
    add(AmbientCue::Crickets, game.run.spawn, true);
    add(AmbientCue::BranchFall, game.run.spawn, true);
    add(AmbientCue::DistantCrow, game.run.spawn, true);
    add(AmbientCue::DistantOwl, game.run.spawn, true);
    for (int i = 0; i < game.run.roof_light_count; ++i) {
        const Cell cell = game.run.roof_lights[static_cast<std::size_t>(i)].cell;
        add(AmbientCue::LeafRustle, cell);
        add(AmbientCue::TinyBird, cell);
        add(AmbientCue::PineCreak, cell);
    }
    for (int y = 1; y < game.stage.height - 1; ++y)
        for (int x = 1; x < game.stage.width - 1; ++x) {
            const Cell cell{x, y};
            const Tile& tile = *game.stage.at(cell);
            if (tile.kind == TileKind::Water) {
                add(AmbientCue::Stream, cell);
                add(AmbientCue::PoolDrips, cell);
                add(AmbientCue::ReedHiss, cell);
                add(AmbientCue::Frogs, cell);
                if (game.stage.at_or_border(cell + Cell{0, -1}).kind == TileKind::Wall)
                    add(AmbientCue::WallTrickle, cell);
            }
            if (tile.prop.broken) continue;
            if (tile.prop.kind == PropKind::RottenLog) {
                add(AmbientCue::BranchCreak, cell, false, {}, tile.prop.kind);
                add(AmbientCue::Woodpecker, cell, false, {}, tile.prop.kind);
            }
            if (tile.prop.kind == PropKind::TallGrass)
                add(AmbientCue::DeerRustle, cell, false, {}, tile.prop.kind);
            if (tile.prop.kind == PropKind::Nest)
                add(AmbientCue::BeeHum, cell, false, {}, tile.prop.kind);
            if (tile.kind == TileKind::Ruin && ((x * 17 + y * 13) % 19 == 0))
                add(AmbientCue::RuinCreak, cell);
        }
    for (int slot = 0; slot < max_entities; ++slot) {
        const Entity& source = game.entities[static_cast<std::size_t>(slot)];
        if (source.kind == EntityKind::Den)
            add(AmbientCue::DenBreath, source.cell, false, {slot, source.generation});
        if (source.kind == EntityKind::Encounter)
            add(AmbientCue::MansionWhisper, source.cell, false, {slot, source.generation});
    }
}
