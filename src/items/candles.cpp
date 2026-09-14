#include "candles.hpp"

namespace {
constexpr RegionalItem candle{"Candle Stub", "Place a small 80s lamp. Recover fuel and condition with pickup. Water snuffs it. Used candles do not stack.",
    Sprite::CandleStub,{1,1,0,0,30,PatternEffect::Utility},
    ItemAction::Material,5,4,true,0,0,0,0,0,SoundId::CandleLight,6};
constexpr RegionalItem wick{"Wick Spool", "Add up to 30s fuel to a candle ahead. Capacity 80s. Does not repair damage or relight a cold wick.",
    Sprite::WickSpool,{1,1,0,0,30,PatternEffect::Utility},
    ItemAction::Material,7,1,false,4,0,0,0,0,SoundId::WickFeed};
} // namespace

const RegionalItem* candle_supply(ItemKind kind) {
    if (kind == ItemKind::CandleStub) return &candle;
    return kind == ItemKind::WickSpool ? &wick : nullptr;
}
