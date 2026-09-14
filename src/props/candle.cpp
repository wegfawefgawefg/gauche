#include "candle.hpp"
#include "growth.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

bool candle_lit(const Prop& prop) {
    return prop.kind == PropKind::Candle && !prop.broken &&
        prop.growth_ticks > 0 && (prop.variant & candle_lit_bit);
}

bool light_candle(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::Candle || tile->prop.broken ||
        tile->prop.growth_ticks == 0 || candle_lit(tile->prop) || surface_wet(*tile)) return false;
    tile->prop.variant |= candle_lit_bit;
    emit_sound(game,SoundId::CandleLight,cell);
    return true;
}

bool douse_candle(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || !candle_lit(tile->prop)) return false;
    tile->prop.variant &= static_cast<std::uint8_t>(~candle_lit_bit);
    emit_sound(game,SoundId::CandleOut,cell);
    return true;
}

bool place_candle(Game& game, Cell cell, const Item& item) {
    if (item.durability <= 0 || !plant_prop(game,cell,PropKind::Candle)) return false;
    Prop& prop = game.stage.at(cell)->prop;
    // FUEL: loaded is remaining ticks; opened distinguishes used candles from unused stacks.
    prop.growth_ticks = static_cast<std::uint16_t>(std::clamp(item.loaded,0,candle_fuel_ticks));
    prop.hp = static_cast<std::uint8_t>(item.durability);
    prop.variant = static_cast<std::uint8_t>((item.loaded > 0 ? candle_lit_bit : 0) |
        (item.attribute == ItemAttribute::Durable ? candle_durable_bit : 0));
    return true;
}

bool refill_candle(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile || tile->prop.kind != PropKind::Candle || tile->prop.broken ||
        tile->prop.growth_ticks >= candle_fuel_ticks) return false;
    tile->prop.growth_ticks = static_cast<std::uint16_t>(std::min(candle_fuel_ticks,
        static_cast<int>(tile->prop.growth_ticks)+1800));
    return true;
}

void step_candle(Game& game, Cell cell) {
    Tile& tile = *game.stage.at(cell);
    if (surface_wet(tile)) douse_candle(game,cell);
    if (!candle_lit(tile.prop)) return;
    if (--tile.prop.growth_ticks == 0) {
        tile.prop.variant &= static_cast<std::uint8_t>(~candle_lit_bit);
        emit_sound(game,SoundId::CandleSpent,cell);
    }
}

Item candle_item(const Prop& prop) {
    if (prop.kind != PropKind::Candle || prop.broken) return {};
    Item item = make_item(ItemKind::CandleStub,1,
        prop.variant & candle_durable_bit ? ItemAttribute::Durable : ItemAttribute::None);
    item.loaded = prop.growth_ticks;
    item.durability = prop.hp;
    item.opened = true;
    return item;
}

int release_candle(Game& game, Cell cell) {
    Tile* tile = game.stage.at(cell);
    if (!tile) return -1;
    const Item item = candle_item(tile->prop);
    if (item.kind == ItemKind::None) return -1;
    const Handle handle = spawn_entity(game,EntityKind::GroundItem,cell);
    Entity* loose = get_entity(game,handle);
    if (!loose) return -1;
    loose->ground_item = item;
    loose->sprite = Sprite::CandleStub;
    // TRANSACTION: The prop remains intact if the loose-item pool is exhausted.
    douse_candle(game,cell);
    tile->prop = {};
    return handle.slot;
}
