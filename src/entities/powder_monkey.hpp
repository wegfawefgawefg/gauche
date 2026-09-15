#pragma once
#include "../game.hpp"
#include <optional>

enum PowderPhase { PowderReady, PowderLight, PowderRun, PowderStrike, PowderRest };
struct PowderSite { Cell cell{}, direction{}, first{}; };
void init_powder_monkey(Entity& actor);
void step_powder_monkey(Game& game,int slot);
void interrupt_powder_monkey(Entity& actor);
std::optional<PowderSite> powder_site(const Game& game,int slot,Cell threat);
std::optional<Cell> powder_escape(const Game& game,int slot,Cell charge,Cell direction);
bool powder_danger(Cell cell,Cell charge,Cell direction);
