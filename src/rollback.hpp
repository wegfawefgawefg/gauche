#pragma once

#include "game.hpp"

#include <array>
#include <cstdint>
#include <deque>
#include <map>
#include <vector>
#include <span>
#include <utility>

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
    std::uint64_t input_commit_tick = 0;
    int max_history = 120;
    bool needs_snapshot = false;
    const char* recovery_reason = "unspecified";
    std::uint64_t recovery_tick = 0;
};

void begin_rollback(RollbackSession& session, const Game& initial);
void predict_frame(RollbackSession& session, const std::array<Input, 4>& inputs);
void confirm_frame(RollbackSession& session, const CanonicalFrame& canonical);
void confirm_host_current(RollbackSession& session);
std::vector<CanonicalFrame> revise_host_input(RollbackSession& session,
                                             std::uint64_t tick, int owner, Input input);
std::vector<CanonicalFrame> revise_host_inputs(RollbackSession& session, int owner,
    std::span<const std::pair<std::uint64_t, Input>> inputs);
void apply_correction_batch(RollbackSession& session,
                            const std::vector<CanonicalFrame>& canonical);
void apply_host_snapshot(RollbackSession& session, const Game& snapshot);
