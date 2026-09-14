#include "glass_eel.hpp"
#include "dispatch.hpp"
#include "../surfaces/conduction.hpp"

#include <algorithm>

namespace {

// SLOTS: label_a swim/charge/rest; timer_a phase; timer_b stranded sound cooldown.
// point_a charge origin; counter_a feeding satiety. No unbounded whole-pool search.
constexpr Cell sides[]{{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

void rest(Entity& eel, int ticks) {
    eel.label_a = EelRest;
    eel.timer_a = ticks;
    eel.sprite = Sprite::EelSpent;
    eel.self_light = {12, 26, 30};
}

bool swim_space(const Game& game, Cell cell) {
    return conductive_cell(game, cell) && entity_at(game, cell, true) < 0;
}

bool swim(Game& game, int slot, Cell cell) {
    Entity& eel = game.entities[static_cast<std::size_t>(slot)];
    if (eel.move_wait > 0 || eel.vitals.rooted > 0 || !swim_space(game, cell)) return false;
    eel.facing = cell - eel.cell;
    eel.cell = cell;
    eel.move_wait = eel.move_interval;
    enter_actor_cell(game, slot);
    return true;
}

const Entity* nearest_player(const Game& game, Cell cell) {
    const Entity* best = nullptr;
    int gap = 10;
    for (int owner = 0; owner < 4; ++owner) {
        const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
        if (!game.run.online[static_cast<std::size_t>(owner)] || !player || player->health <= 0) continue;
        const int candidate = distance(player->cell, cell);
        if (candidate < gap) { gap = candidate; best = player; }
    }
    return best;
}

bool player_in_water(const Game& game, const WetWave& wave) {
    for (int owner = 0; owner < 4; ++owner) {
        const Entity* player = get_entity(game, game.players[static_cast<std::size_t>(owner)]);
        if (!game.run.online[static_cast<std::size_t>(owner)] || !player || !water_shock_target(game, *player)) continue;
        for (int i = 0; i < wave.count; ++i)
            if (player->cell == wave.nodes[static_cast<std::size_t>(i)].cell) return true;
    }
    return false;
}

void seek_bank(Game& game, int slot, const Entity* target) {
    Entity& eel = game.entities[static_cast<std::size_t>(slot)];
    if (eel.move_wait > 0 || eel.vitals.rooted > 0) return;
    const int rotation = static_cast<int>(random_u32(game) % 4);
    if (target) {
        WetWave route;
        route.nodes[0] = {eel.cell, 0, 0};
        route.count = 1;
        int best = 0, gap = distance(eel.cell, target->cell);
        // SWIM: Occupied cells block travel but not electricity. Look around local obstacles.
        for (int next = 0; next < route.count; ++next) {
            const WetNode node = route.nodes[static_cast<std::size_t>(next)];
            if (node.steps >= 6) continue;
            for (int side = 0; side < 4; ++side) {
                const Cell cell = node.cell + sides[(side + rotation) % 4];
                if (!swim_space(game, cell)) continue;
                bool seen = false;
                for (int i = 0; i < route.count; ++i)
                    if (route.nodes[static_cast<std::size_t>(i)].cell == cell) { seen = true; break; }
                if (seen || route.count >= static_cast<int>(route.nodes.size())) continue;
                const int index = route.count++;
                route.nodes[static_cast<std::size_t>(index)] = {cell, next, node.steps + 1};
                const int candidate = distance(cell, target->cell);
                if (candidate < gap) { best = index; gap = candidate; }
            }
        }
        if (best > 0) {
            while (route.nodes[static_cast<std::size_t>(best)].parent != 0)
                best = route.nodes[static_cast<std::size_t>(best)].parent;
            if (swim(game, slot, route.nodes[static_cast<std::size_t>(best)].cell)) return;
        }
    }
    for (int side = 0; side < 4; ++side)
        if (swim(game, slot, eel.cell + sides[(side + rotation) % 4])) return;
    eel.move_wait = 18;
}

// BAIT: A connected swim route wins over a new shock, never a committed charge.
bool follow_bait(Game& game, int slot) {
    Entity& eel = game.entities[static_cast<std::size_t>(slot)];
    if (eel.counter_a > 0) return false;
    std::array<int,max_entities> food{};
    int count = 0;
    for (int i=0;i<max_entities;++i) {
        const Entity& candidate = game.entities[static_cast<std::size_t>(i)];
        if (candidate.kind == EntityKind::GroundItem && candidate.ground_item.kind == ItemKind::SmokedFish &&
            candidate.ground_item.count > 0 && distance(candidate.cell,eel.cell) <= 6)
            food[static_cast<std::size_t>(count++)] = i;
    }
    if (count == 0) return false;
    WetWave route;
    route.nodes[0] = {eel.cell,0,0}; route.count = 1;
    for (int next=0;next<route.count;++next) {
        const WetNode node = route.nodes[static_cast<std::size_t>(next)];
        for (int i=0;i<count;++i) {
            Entity& bait = game.entities[static_cast<std::size_t>(food[static_cast<std::size_t>(i)])];
            if (bait.cell != node.cell) continue;
            if (next == 0) {
                if (--bait.ground_item.count == 0) remove_entity(game,{food[static_cast<std::size_t>(i)],bait.generation});
                eel.health = std::min(eel.max_health,eel.health+4);
                eel.counter_a = 300;
                rest(eel,90);
                emit_sound(game,SoundId::FishNibble,eel.cell);
            } else if (eel.move_wait == 0) {
                int first = next;
                while (route.nodes[static_cast<std::size_t>(first)].parent != 0)
                    first = route.nodes[static_cast<std::size_t>(first)].parent;
                swim(game,slot,route.nodes[static_cast<std::size_t>(first)].cell);
            }
            return true;
        }
        if (node.steps >= 6) continue;
        for (Cell side : sides) {
            const Cell cell = node.cell+side;
            if (!swim_space(game,cell)) continue;
            bool seen = false;
            for (int i=0;i<route.count;++i)
                if (route.nodes[static_cast<std::size_t>(i)].cell == cell) { seen = true; break; }
            if (!seen && route.count < static_cast<int>(route.nodes.size()))
                route.nodes[static_cast<std::size_t>(route.count++)] = {cell,next,node.steps+1};
        }
    }
    return false;
}

} // namespace

void init_glass_eel(Entity& eel) {
    eel.health = eel.max_health = 42;
    eel.move_interval = 15;
    eel.impassable = true;
    eel.sprite = Sprite::GlassEel;
    eel.self_light = {12, 26, 30};
}

void interrupt_glass_eel(Entity& eel) {
    if (eel.kind == EntityKind::GlassEel && eel.label_a == EelCharge) rest(eel, 90);
}

void step_glass_eel(Game& game, int slot) {
    Entity& eel = game.entities[static_cast<std::size_t>(slot)];
    eel.counter_a = std::max(0,eel.counter_a-1);
    if (!conductive_cell(game, eel.cell)) {
        interrupt_glass_eel(eel);
        eel.sprite = Sprite::EelStranded;
        // STRANDED: Freeze or drain its pool. It can flop into adjacent water, never cross land.
        if (eel.timer_b == 0) { emit_sound(game, SoundId::EelFlop, eel.cell); eel.timer_b = 90; }
        seek_bank(game, slot, nullptr);
        return;
    }
    if (eel.label_a == EelRest) {
        eel.sprite = Sprite::EelSpent;
        if (eel.timer_a == 0) { eel.label_a = EelSwim; eel.sprite = Sprite::GlassEel; }
        return;
    }
    if (eel.label_a == EelCharge) {
        if (eel.cell != eel.point_a || eel.freeze_ticks > 0) { rest(eel, 90); return; }
        if (eel.timer_a == 0) {
            discharge_water(game, eel.cell, eel.cell, 18, eel_shock_reach);
            emit_sound(game, SoundId::EelDischarge, eel.cell);
            rest(eel, 90);
        }
        return;
    }
    if (follow_bait(game,slot)) return;
    if (eel.freeze_ticks == 0 && player_in_water(game, wet_wave(game, eel.cell, eel_shock_reach))) {
        eel.label_a = EelCharge;
        eel.timer_a = 48;
        eel.point_a = eel.cell;
        eel.sprite = Sprite::EelCharge;
        eel.self_light = {90, 152, 180};
        emit_sound(game, SoundId::EelCharge, eel.cell);
        return;
    }
    eel.sprite = Sprite::GlassEel;
    seek_bank(game, slot, nearest_player(game, eel.cell));
}
