#include "snow_tools.hpp"
#include "../world/snow.hpp"
#include "../entities/attacks.hpp"

#include <algorithm>

namespace {

constexpr RegionalItem scoop{"Snow Scoop", "Clear snow and expose burrowers or buried supplies. Collects snow to pack into balls. Glue repairs it.",
    Sprite::SnowScoop, {1, 1, 0, 0, 24, PatternEffect::Utility, false, 1},
    ItemAction::Material, 9, 1, false, 0, 0, 0, 0, 0, SoundId::SnowScrape, 60};
constexpr RegionalItem ball{"Snowball", "A small thrown distraction. Wets on impact, douses flame and disrupts fragile windups. Breaks on landing.",
    Sprite::Snowball, {1, 5, 0, 1, 18, PatternEffect::Damage, true},
    ItemAction::Throw, 2, 12, true, 0, 0, 0, 0, 0, SoundId::SnowThrow};

} // namespace

const RegionalItem* snow_tool_item(ItemKind kind) {
    if (kind == ItemKind::SnowScoop) return &scoop;
    if (kind == ItemKind::Snowball) return &ball;
    return nullptr;
}

bool use_snow_scoop(Game& game, int slot, Cell direction) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    Item& item = *actor.inventory.held();
    const ItemPattern pattern = item_pattern(item);
    const Cell side{-direction.y, direction.x};
    int cleared = 0;
    for (int lane = -pattern.half_width; lane <= pattern.half_width; ++lane) {
        const Cell cell = actor.cell + direction + Cell{side.x * lane, side.y * lane};
        const bool cache = game.stage.at_or_border(cell).prop.kind == PropKind::SnowCache;
        if (!clear_attack_sight(game, actor.cell, cell, false) || !clear_snow(game, cell)) continue;
        ++cleared;
        if (!cache && game.impact_count < static_cast<int>(game.impacts.size()))
            game.impacts[static_cast<std::size_t>(game.impact_count++)] = {cell, actor.cell, Sprite::DebrisSnowClump, 1};
    }
    if (cleared == 0) return false;
    // CARGO: loaded is this tool's collected snow, not gun ammunition. Packing spends one unit.
    item.loaded = std::min(scoop_snow_capacity, item.loaded + cleared);
    if (game.sweep_count < static_cast<int>(game.sweeps.size()))
        game.sweeps[static_cast<std::size_t>(game.sweep_count++)] = {actor.cell, direction, 1, pattern.half_width, true};
    return true;
}

bool pack_snowball(Game& game, int slot) {
    Entity& actor = game.entities[static_cast<std::size_t>(slot)];
    Item& scoop_item = *actor.inventory.held();
    if (scoop_item.kind != ItemKind::SnowScoop || scoop_item.cooldown > 0 || scoop_item.loaded <= 0) return false;
    if (!insert_item(actor.inventory, make_item(ItemKind::Snowball))) return false;
    --scoop_item.loaded;
    scoop_item.cooldown = 12;
    emit_sound(game, SoundId::SnowPack, actor.cell);
    return true;
}
