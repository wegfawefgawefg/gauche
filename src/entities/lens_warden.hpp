#pragma once
#include "../game.hpp"
#include "../combat/beams.hpp"

enum WardenPhase { WardenIdle, WardenTurn, WardenCharge, WardenRecover };
void init_lens_warden(Entity& warden);
void step_lens_warden(Game& game, int slot);
void interrupt_lens_warden(Entity& warden);
bool warden_charging(const Game& game, const Entity& warden);
BeamTrace warden_beam(const Game& game, const Entity& warden);
std::vector<Cell> charged_warden_lamps(const Game& game);
Handle spawn_lens_watch(Game& game, Cell stand, Cell lamp, Cell mirror);
