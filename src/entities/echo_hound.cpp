#include "echo_hound.hpp"
#include "behavior.hpp"
#include "hearing.hpp"
#include "attacks.hpp"

namespace {

// SLOTS: label_a phase; timer_a phase/trail deadline; point_a remembered destination.
// point_b committed bite; counter_a/b bite origin x/y; c-slots pending sound memory.
// The pending sound can change, but never bends a trail or bite already underway.
void listen(Entity& hound, int ticks) {
    hound.label_a = EchoListen;
    hound.timer_a = ticks;
    hound.sprite = Sprite::EchoHound;
}

void begin_bite(Game& game, Entity& hound) {
    hound.label_a = EchoBite;
    hound.timer_a = 24;
    hound.point_b = hound.point_a;
    hound.counter_a = hound.cell.x; hound.counter_b = hound.cell.y;
    hound.facing = cardinal_toward(hound.cell, hound.point_b, hound.facing);
    hound.sprite = Sprite::EchoHoundWarn;
    emit_sound(game, SoundId::EchoWarn, hound.cell);
}

} // namespace

void init_echo_hound(Entity& hound) {
    hound.health = hound.max_health = 44;
    hound.move_interval = 9;
    hound.impassable = true;
    listen(hound, 0);
}

void interrupt_echo_hound(Entity& hound) {
    if (hound.kind != EntityKind::EchoHound || hound.health <= 0) return;
    hound.label_a = EchoRecover;
    hound.timer_a = 45;
    hound.sprite = Sprite::EchoHoundRecover;
}

void step_echo_hound(Game& game, int slot) {
    Entity& hound = game.entities[static_cast<std::size_t>(slot)];
    if (hound.label_a == EchoBite) {
        if (hound.cell != Cell{hound.counter_a, hound.counter_b}) { interrupt_echo_hound(hound); return; }
        if (hound.timer_a > 0) return;
        // EMPTY AIR: Still bite the heard cell even if its occupant already left.
        resolve_enemy_attack(game, slot, 14, SoundId::EchoBite);
        interrupt_echo_hound(hound);
        return;
    }
    if (hound.label_a == EchoRecover) {
        if (hound.timer_a == 0) listen(hound, 24);
        return;
    }
    if (hound.label_a == EchoListen) {
        if (hound.timer_a > 0 || hound.timer_c == 0 || hound.label_c != InvestigateNoise) return;
        hound.point_a = hound.point_c;
        hound.label_c = hound.timer_c = 0;
        hound.label_a = EchoTrail;
        hound.timer_a = 180;
        hound.sprite = Sprite::EchoHoundTrail;
        hound.facing = cardinal_toward(hound.cell, hound.point_a, hound.facing);
        emit_sound(game, SoundId::EchoListen, hound.cell);
        return;
    }
    if (hound.cell == hound.point_a || hound.timer_a == 0) { listen(hound, 30); return; }
    if (distance(hound.cell, hound.point_a) == 1 && clear_attack_sight(game, hound.cell, hound.point_a, false)) {
        begin_bite(game, hound); return;
    }
    if (hound.move_wait > 0) return;
    const auto next = next_route_cell(game, slot, hound.point_a, 512);
    if (!next || !willing_step(game, slot, *next)) listen(hound, 30);
}
