#include "boiler_tank.hpp"
#include "../surfaces/interaction.hpp"
#include "../surfaces/temperature.hpp"

#include <algorithm>

int boiler_at(const Game& game, Cell cell) {
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& tank = game.entities[static_cast<std::size_t>(slot)];
        if (tank.kind == EntityKind::BoilerTank && tank.health > 0 && tank.cell == cell) return slot;
    }
    return -1;
}

// SLOTS: counter_a pressure 0..100, counter_b fuel ticks, counter_c leaking flag.
// label_a phase, label_b already ruptured; timer_a tell/vent, timer_b sealed outlet.
// point_a committed origin, point_b committed vent direction. facing is the next
// nozzle direction, fixed by ground_item when a real Pressure Valve is attached.
void init_boiler_tank(Entity& tank) {
    tank.health = tank.max_health = 60;
    tank.impassable = tank.hard_blocker = true;
    tank.counter_b = 3600;
    tank.sprite = Sprite::BoilerTank;
}

bool arm_boiler(Game& game, Entity& tank) {
    if (tank.health <= 0 || tank.label_a != BoilerIdle || tank.timer_b > 0 || tank.counter_a < 60) return false;
    tank.point_a = tank.cell; tank.point_b = tank.facing;
    tank.label_a = BoilerTell; tank.timer_a = 45;
    tank.sprite = Sprite::BoilerTell;
    emit_sound(game,SoundId::BoilerWarn,tank.cell);
    return true;
}

bool feed_boiler(Game& game, Cell cell) {
    const int slot = boiler_at(game,cell);
    if (slot < 0) return false;
    Entity& tank = game.entities[static_cast<std::size_t>(slot)];
    if (tank.counter_b >= boiler_fuel_limit) return false;
    tank.counter_b = std::min(boiler_fuel_limit,tank.counter_b+1200);
    emit_sound(game,SoundId::CoalFeed,cell);
    return true;
}

void damage_boiler(Game& game, int slot) {
    Entity& tank = game.entities[static_cast<std::size_t>(slot)];
    if (tank.kind != EntityKind::BoilerTank) return;
    if (tank.health <= 0) { rupture_boiler(game,slot); return; }
    if (!tank.counter_c) emit_sound(game,SoundId::BoilerLeak,tank.cell);
    tank.counter_c = 1;
}

void step_boiler_tank(Game& game, int slot) {
    Entity& tank = game.entities[static_cast<std::size_t>(slot)];
    const Tile& tile = game.stage.at_or_border(tank.cell);
    if (tank.counter_b > 0) {
        --tank.counter_b;
        if (game.tick%3 == 0) tank.counter_a = std::min(100,tank.counter_a+1);
    } else if (game.tick%6 == 0) tank.counter_a = std::max(0,tank.counter_a-1);
    // COOLING: Water condenses pressure without deleting the remaining coal supply.
    if (surface_wet(tile)) tank.counter_a = std::max(0,tank.counter_a-2);
    const bool leaking = tank.health < tank.max_health && tank.timer_b == 0;
    tank.counter_c = leaking ? 1 : 0;
    if (leaking && game.tick%6 == 0) tank.counter_a = std::max(0,tank.counter_a-1);
    if (leaking && tank.counter_a > 0 && game.tick%90 == 0) emit_sound(game,SoundId::BoilerLeak,tank.cell);
    tank.light = tank.counter_b > 0 ? LightEmitter{2,450,{237,161,75}} : LightEmitter{};
    tank.self_light = {};
    if (tank.label_a == BoilerTell) {
        if (tank.cell != tank.point_a || tank.counter_a < 25 || tank.timer_b > 0) {
            tank.label_a = BoilerVent; tank.timer_a = 30;
        } else if (tank.timer_a == 0) {
            boiler_splash(game,slot,false);
            if (tank.health <= 0) return;
            tank.counter_a = 0; tank.label_a = BoilerVent; tank.timer_a = 30;
            emit_sound(game,SoundId::BoilerVent,tank.cell);
        }
    } else if (tank.label_a == BoilerVent) {
        if (tank.timer_a == 0) tank.label_a = BoilerIdle;
    } else if (tank.counter_a == 100) arm_boiler(game,tank);
    tank.sprite = tank.timer_b > 0 ? Sprite::BoilerPlugged : tank.label_a == BoilerTell ? Sprite::BoilerTell :
        tank.counter_a >= 60 ? Sprite::BoilerHot : Sprite::BoilerTank;
    if (tank.label_a == BoilerTell) tank.self_light = {36,21,8};
}

// SNAPSHOTS: The compact codec delegates the vessel's domain-specific bounds here.
bool valid_boiler_state(const Entity& actor) {
    if (actor.kind == EntityKind::BoilerPorter) return actor.label_a >= 0 && actor.label_a <= 3;
    if (actor.kind != EntityKind::BoilerTank) return true;
    return actor.counter_a >= 0 && actor.counter_a <= 100 &&
        actor.counter_b >= 0 && actor.counter_b <= boiler_fuel_limit &&
        actor.counter_c >= 0 && actor.counter_c <= 1 && actor.label_b >= 0 && actor.label_b <= 1 &&
        actor.label_a >= BoilerIdle && actor.label_a <= BoilerVent &&
        actor.timer_a >= 0 && actor.timer_a <= 45 && actor.timer_b >= 0 && actor.timer_b <= 600 &&
        distance({},actor.facing) == 1 && (actor.label_a != BoilerTell || distance({},actor.point_b) == 1) &&
        (actor.ground_item.kind == ItemKind::None || actor.ground_item.kind == ItemKind::PressureValve);
}
