#include "alarms.hpp"

namespace {

constexpr RegionalItem alarm_clock{"Alarm Clock", "Place to lure listeners after 3s. Rings for 5s. Recover with remaining condition. Can be destroyed.",
    Sprite::AlarmClock, {1, 1, 0, 0, 30, PatternEffect::Utility},
    ItemAction::Material, 10, 1, true, 0, 0, 0, 0, 0, SoundId::ClockWind, 8};

} // namespace

const RegionalItem* alarm_item(ItemKind kind) {
    return kind == ItemKind::AlarmClock ? &alarm_clock : nullptr;
}
