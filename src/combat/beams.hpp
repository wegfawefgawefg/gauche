#pragma once
#include "../game.hpp"

struct BeamCell {
    Cell from{}, cell{};
    int damage = 0;
    Handle target{};
    bool optic = false, stop = false, reflected = false;
};
struct BeamTrace {
    std::array<BeamCell, 128> cells{};
    int count = 0;
};

bool optical_prop(const Prop& prop);
BeamTrace trace_beam(const Game& game, Cell source, Cell direction, int damage, int reach, bool piercing);
BeamTrace trace_beam_burst(const Game& game, Cell source, int damage, int reach, bool piercing);
void resolve_beam(Game& game, const BeamTrace& trace);
