#pragma once
#include "../game.hpp"

struct WetNode { Cell cell{}; int parent = 0; int steps = 0; };
struct WetWave {
    std::array<WetNode, 85> nodes{}; // Six-step cardinal diamond, including its origin.
    int count = 0;
    Cell ground{};
    int ground_node = -1;
};
struct ShockVictim { Handle handle; int steps = 0; };
struct ConductedShock {
    WetWave wave;
    std::vector<ShockVictim> victims;
    int reach = 0;
};

bool conductive_water(const Game& game, Cell cell);
bool conductive_cell(const Game& game, Cell cell);
bool water_shock_target(const Game& game, const Entity& actor);
WetWave wet_wave(const Game& game, Cell origin, int reach);
ConductedShock trace_conducted_shock(const Game& game, Cell origin, int reach);
void apply_conducted_shock(Game& game, const ConductedShock& shock, Cell attacker, int damage);
void discharge_water(Game& game, Cell origin, Cell attacker, int damage, int reach);
