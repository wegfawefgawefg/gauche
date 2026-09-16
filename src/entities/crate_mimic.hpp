#pragma once
#include "../game.hpp"
// label_a phase; timer_a bite/recovery; timer_b unseen; counter_a already rehidden.
// point_a home; point_b locked bite cell; facing locks its adjacent origin.
enum MimicPhase { MimicHidden, MimicWindup, MimicRetreat, MimicHunt, MimicRecover };
constexpr int mimic_windup_ticks=24, mimic_recovery_ticks=18;
void interrupt_mimic_bite(Entity& mimic);
bool valid_mimic(const Entity& mimic);
