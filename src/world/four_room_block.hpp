#pragma once
#include "route.hpp"
#include <optional>
// Pick a 2x2 block inside existing bounds, reserve existing corners, and connect
// missing corners. Objectives, secret rooms and earlier landmarks are excluded.
std::optional<Cell> reserve_four_rooms(Game& game,FloorPlan& plan,std::array<int,4>& rooms);
