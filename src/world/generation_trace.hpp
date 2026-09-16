#pragma once
#include "route.hpp"
#include <memory>
#include <string>

// Optional inspection capture: no decisions, random draws, or gameplay ownership.
// Coarse checkpoints are deliberately bounded; the finished Game is kept separately.
struct GenerationCheckpoint {
    std::string name;
    std::unique_ptr<Game> game;
    std::vector<RoomPlan> rooms;
};
struct GenerationTrace {
    static constexpr std::size_t limit = 24;
    std::vector<GenerationCheckpoint> checkpoints;
    bool truncated = false;
    void capture(const char* name, const Game& game, const FloorPlan& plan) {
        if (checkpoints.size() >= limit) { truncated = true; return; }
        checkpoints.push_back({name, std::make_unique<Game>(game), plan.rooms});
    }
};
