#include "gate.hpp"
#include "../props/doorstop.hpp"

namespace {
bool occupied(const Game& game, const Entity& gate) {
    for (const Entity& actor : game.entities)
        if (&actor != &gate && actor.kind != EntityKind::None && actor.impassable &&
            actor.health > 0 && actor.cell == gate.cell) return true;
    return false;
}
} // namespace

// SLOTS: label_a requested closed; label_b 0 encounter / 1 timed sluice.
// counter_a/b open/closed durations; timer_a cycle beat. Occupancy/wedges defer closure.
void request_gate(Game& game, Entity& gate, bool open) {
    gate.label_a = open ? 0 : 1;
    const bool held = doorstop_present(game.stage.at_or_border(gate.cell).prop);
    const bool actual_open = open || held || occupied(game,gate);
    if (gate.fixture_open != actual_open)
        emit_sound(game,actual_open ? SoundId::SluiceOpen : SoundId::SluiceClose,gate.cell);
    gate.fixture_open = actual_open;
    gate.impassable = gate.hard_blocker = !actual_open;
    gate.light = actual_open ? LightEmitter{} : LightEmitter{2,400,{230,78,61}};
}

void configure_timed_gate(Entity& gate) {
    gate.label_b = 1;
    gate.label_a = 0;
    gate.counter_a = 180; gate.counter_b = 120;
    gate.timer_a = gate.counter_a;
    gate.sprite = Sprite::SluiceGate;
    gate.fixture_open = true;
    gate.impassable = gate.hard_blocker = false;
}

void step_gate(Game& game, int slot) {
    Entity& gate = game.entities[static_cast<std::size_t>(slot)];
    if (gate.label_b == 1) {
        if (gate.label_a == 0 && gate.timer_a == 30)
            emit_sound(game,SoundId::SluiceWarning,gate.cell);
        if (gate.timer_a == 0) {
            gate.label_a = gate.label_a == 0 ? 1 : 0;
            gate.timer_a = gate.label_a == 0 ? gate.counter_a : gate.counter_b;
            if (gate.label_a != 0 && doorstop_present(game.stage.at_or_border(gate.cell).prop))
                emit_sound(game,SoundId::WedgeStrain,gate.cell);
        }
    }
    request_gate(game,gate,gate.label_a == 0);
}

bool valid_gate_state(const Entity& gate) {
    if (gate.kind != EntityKind::EncounterGate) return true;
    if (gate.label_a < 0 || gate.label_a > 1 || gate.label_b < 0 || gate.label_b > 1) return false;
    if (gate.impassable != !gate.fixture_open || gate.hard_blocker != !gate.fixture_open) return false;
    return gate.label_b == 0 || (gate.counter_a >= 60 && gate.counter_a <= 600 &&
        gate.counter_b >= 60 && gate.counter_b <= 600 && gate.timer_a <= 600);
}
