#pragma once

#include "game.hpp"

#include <array>
#include <cstdint>
#include <deque>
#include <map>

struct CanonicalFrame {
    std::uint64_t tick = 0;
    std::array<Input, 4> inputs{};
    std::uint64_t hash = 0;
};

struct RollbackFrame {
    Game before{};
    std::array<Input, 4> inputs{};
    std::uint64_t tick = 0;
    std::uint64_t hash_after = 0;
    std::uint64_t host_hash = 0;
    bool confirmed = false;
};

struct RollbackSession {
    Game game{};
    std::deque<RollbackFrame> frames;
    std::map<std::uint64_t, CanonicalFrame> pending;
    std::uint64_t confirmed_through = 0;
    std::uint64_t rollback_count = 0;
    int max_history = 120;
    bool needs_snapshot = false;
};

void begin_rollback(RollbackSession& session, const Game& initial);
void predict_frame(RollbackSession& session, const std::array<Input, 4>& inputs);
void confirm_frame(RollbackSession& session, const CanonicalFrame& canonical);
void apply_host_snapshot(RollbackSession& session, const Game& snapshot);
