#include "boiler_porter.hpp"
#include "boiler_tank.hpp"
#include "behavior.hpp"
#include "attacks.hpp"
#include "../surfaces/interaction.hpp"

#include <algorithm>

namespace {
// SLOTS: entity_a tank; label_a phase; timer_a committed push/jab/recovery.
// point_a starting stance, point_b destination/strike; facing committed direction.
constexpr Cell sides[]{{1,0},{0,1},{-1,0},{0,-1}};
void rest(Entity& porter, int ticks) {
    porter.label_a = PorterRest; porter.timer_a = ticks;
    porter.sprite = Sprite::BoilerPorter; porter.self_light = {};
}

bool clear_space(const Game& game, Cell cell, int except=-1) {
    const Tile* tile = game.stage.at(cell);
    if (!tile || !walkable(*tile) || tile->kind == TileKind::Lava) return false;
    for (int slot=0;slot<max_entities;++slot) {
        const Entity& actor = game.entities[static_cast<std::size_t>(slot)];
        if (slot != except && actor.kind != EntityKind::None && actor.cell == cell &&
            (actor.impassable || actor.kind == EntityKind::GroundItem)) return false;
    }
    return true;
}

void maneuver(Game& game, int slot, Entity& tank, Cell target) {
    Entity& porter = game.entities[static_cast<std::size_t>(slot)];
    int best = 100000;
    Cell push{}, stand{};
    for (Cell direction : sides) {
        const Cell at = tank.cell-direction, destination = tank.cell+direction;
        if (!clear_space(game,at,slot) || !clear_space(game,destination)) continue;
        if (at != porter.cell && !next_route_cell(game,slot,at,192)) continue;
        const int score = distance(destination,target)*4+distance(porter.cell,at);
        if (score < best) { best = score; push = direction; stand = at; }
    }
    if (best == 100000) { rest(porter,30); return; }
    if (porter.cell != stand) {
        if (porter.move_wait == 0) {
            const auto next = next_route_cell(game,slot,stand,192);
            if (!next || !willing_step(game,slot,*next)) rest(porter,30);
        }
        return;
    }
    if (porter.move_wait > 0 || porter.vitals.rooted > 0 || tank.label_a != BoilerIdle) return;
    porter.label_a = PorterPush; porter.timer_a = 18;
    porter.point_a = porter.cell; porter.point_b = tank.cell+push; porter.facing = push;
    porter.sprite = Sprite::PorterPush;
    emit_sound(game,SoundId::PorterHeave,porter.cell);
}
}

void init_boiler_porter(Entity& porter) {
    porter.health = porter.max_health = 68;
    porter.move_interval = 18; porter.impassable = true;
    porter.sprite = Sprite::BoilerPorter;
}

Handle spawn_boiler_porter(Game& game, Cell cell) {
    if (!clear_space(game,cell)) return {};
    for (Cell direction : sides) {
        if (!clear_space(game,cell+direction) || surface_wet(game.stage.at_or_border(cell+direction))) continue;
        const Handle owner = spawn_entity(game,EntityKind::BoilerPorter,cell);
        Entity* porter = get_entity(game,owner);
        if (!porter) return {};
        const Handle vessel = spawn_entity(game,EntityKind::BoilerTank,cell+direction);
        Entity* tank = get_entity(game,vessel);
        if (!tank) { remove_entity(game,owner); return {}; }
        porter->entity_a = vessel; tank->entity_a = owner; tank->facing = direction;
        return owner;
    }
    return {};
}

void interrupt_boiler_porter(Entity& porter) {
    if (porter.kind != EntityKind::BoilerPorter || porter.health <= 0) return;
    rest(porter,60);
}

void step_boiler_porter(Game& game, int slot) {
    Entity& porter = game.entities[static_cast<std::size_t>(slot)];
    Entity* tank = get_entity(game,porter.entity_a);
    if (tank && (tank->kind != EntityKind::BoilerTank || tank->health <= 0)) tank = nullptr;
    if (porter.label_a == PorterRest) {
        if (porter.timer_a > 0) return;
        porter.label_a = PorterReady;
    }
    if (porter.label_a == PorterBite || porter.label_a == PorterPush) {
        if (porter.cell != porter.point_a || porter.vitals.rooted > 0) { rest(porter,60); return; }
        if (porter.timer_a > 0) return;
        if (porter.label_a == PorterBite) resolve_enemy_attack(game,slot,14,SoundId::PorterHit);
        else if (tank && tank->label_a == BoilerIdle && tank->cell == porter.cell+porter.facing &&
            tank->cell+porter.facing == porter.point_b && clear_space(game,porter.point_b)) {
            const Cell old = tank->cell;
            tank->cell = porter.point_b;
            move_entity(game,slot,old);
            emit_sound(game,SoundId::BoilerRoll,tank->cell);
        }
        if (porter.health > 0) rest(porter,24);
        return;
    }
    const auto close = enemy_target(game,porter.cell,1);
    if (close && distance(porter.cell,close->cell) == 1 && clear_attack_sight(game,porter.cell,close->cell)) {
        porter.facing = close->cell-porter.cell;
        porter.point_a = porter.cell; porter.point_b = close->cell;
        porter.label_a = PorterBite; porter.timer_a = 30;
        porter.sprite = Sprite::PorterWarn;
        emit_sound(game,SoundId::PorterWarn,porter.cell);
        return;
    }
    const auto target = enemy_target(game,tank ? tank->cell : porter.cell,8);
    if (!target || !clear_attack_sight(game,tank ? tank->cell : porter.cell,target->cell)) return;
    if (!tank) { porter.move_interval = 12; pursue(game,slot,target->cell); return; }
    if (tank->ground_item.kind != ItemKind::PressureValve)
        tank->facing = cardinal_toward(tank->cell,target->cell,tank->facing);
    if (distance(tank->cell,target->cell) <= 4 &&
        (tank->cell.x == target->cell.x || tank->cell.y == target->cell.y) && arm_boiler(game,*tank)) return;
    maneuver(game,slot,*tank,target->cell);
}
