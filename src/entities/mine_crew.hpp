#pragma once
#include "../game.hpp"
#include <optional>

enum CrewPhase { CrewReady, CrewCut, CrewStrike, CrewRest, CrewWhistle };
constexpr bool mine_worker(EntityKind kind) {
    return kind==EntityKind::Pickhand || kind==EntityKind::ShiftForeman;
}
void init_mine_worker(Entity& worker);
void step_mine_worker(Game& game,int slot);
void interrupt_mine_worker(Entity& worker);
void hurt_mine_worker(Game& game,int slot,int damage,Cell source);
bool crew_diggable(const Tile& tile);
std::optional<Cell> mine_route_step(const Game& game,int slot,Cell target);

void alarm_mine_workers(Game& game,Cell origin,Handle threat);
