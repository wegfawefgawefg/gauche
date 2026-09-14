#include "alarm_clock.hpp"
#include "growth.hpp"
#include "../entities/hearing.hpp"
#include "../world/ground_items.hpp"

namespace {

void recover_clock(Game& game, Cell cell, Prop& prop) {
    // CAPACITY: Keep a stopped clock intact until a loose-item slot is available.
    const Cell destination = nearby_ground_item_cell(game, cell);
    Entity* loose = get_entity(game, spawn_entity(game, EntityKind::GroundItem, destination));
    if (!loose) return;
    loose->ground_item = make_item(ItemKind::AlarmClock, 1,
        prop.variant == 1 ? ItemAttribute::Durable : ItemAttribute::None);
    loose->ground_item.durability = prop.hp;
    loose->sprite = Sprite::AlarmClock;
    prop = {};
    emit_sound(game, SoundId::ClockStop, cell);
}

} // namespace

bool place_alarm_clock(Game& game, Cell cell, const Item& item) {
    if (item.durability <= 0 || !plant_prop(game, cell, PropKind::AlarmClock)) return false;
    Prop& prop = game.stage.at(cell)->prop;
    // SLOTS: variant 0 ordinary / 1 Durable; hp retains actual carried condition.
    prop.variant = item.attribute == ItemAttribute::Durable ? 1 : 0;
    prop.hp = static_cast<std::uint8_t>(item.durability);
    prop.growth_ticks = alarm_delay_ticks + alarm_ring_ticks;
    return true;
}

void step_alarm_clock(Game& game, Cell cell) {
    Prop& prop = game.stage.at(cell)->prop;
    if (prop.broken) return;
    if (prop.growth_ticks > 0) --prop.growth_ticks;
    if (prop.growth_ticks == 0) { recover_clock(game, cell, prop); return; }
    if (prop.growth_ticks > alarm_ring_ticks) {
        if (prop.growth_ticks % 30 == 0) emit_sound(game, SoundId::ClockTick, cell);
        return;
    }
    // ALARM: Each one-second pulse uses real acoustic propagation, never local playback.
    if (prop.growth_ticks % 60 == 0) {
        make_noise(game, cell, 10);
        emit_sound(game, SoundId::ClockRing, cell);
    }
}

Sprite alarm_clock_sprite(const Prop& prop) {
    if (prop.growth_ticks == 0) return Sprite::AlarmClock;
    if (prop.growth_ticks > alarm_ring_ticks) return Sprite::AlarmClockWound;
    return prop.growth_ticks % 12 < 6 ? Sprite::AlarmClockRinging : Sprite::AlarmClockWound;
}
