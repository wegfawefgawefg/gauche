#pragma once

#include "../game.hpp"
#include <vector>

enum AttentionMode { NoAttention, InvestigateNoise, StartleNoise, FollowScent, SniffScent, WhistleHunt, PlayerWorkOrder };

// SHARED SLOTS: point_c origin, label_c attention mode, timer_c memory.
// PlayerWorkOrder uses point_c as a fixed mining destination until timer_c expires.
// Each species calls step_hearing at an interruptible point in its own step.
std::vector<Cell> audible_cells(const Game& game, Cell origin, int radius);
void make_noise(Game& game, Cell origin, int radius, int startle_radius = 0);
bool step_hearing(Game& game, int slot);
