#include "rollback.hpp"

#include <algorithm>

namespace {

void check_confirmed(RollbackSession& session) {
    for (const RollbackFrame& frame : session.frames) {
        if (frame.tick <= session.confirmed_through) continue;
        if (frame.tick != session.confirmed_through + 1 || !frame.confirmed) break;
        if (frame.hash_after != frame.host_hash) {
            session.needs_snapshot = true;
            session.recovery_reason = "confirmed_hash_mismatch";
            session.recovery_tick = frame.tick;
            return;
        }
        session.confirmed_through = frame.tick;
    }
}

void replay_from(RollbackSession& session, std::size_t index) {
    session.game = session.frames[index].before;
    for (std::size_t replay = index; replay < session.frames.size(); ++replay) {
        RollbackFrame& frame = session.frames[replay];
        frame.before = session.game;
        step_game(session.game, frame.inputs);
        frame.hash_after = game_hash(session.game);
    }
    session.game.sound_count = 0;
    ++session.rollback_count;
}

} // namespace

void begin_rollback(RollbackSession& session, const Game& initial) {
    session = {};
    session.game = initial;
    session.confirmed_through = initial.tick;
}

void predict_frame(RollbackSession& session, const std::array<Input, 4>& inputs) {
    if (session.needs_snapshot) return;
    const std::uint64_t tick = session.game.tick + 1;
    RollbackFrame frame;
    frame.before = session.game;
    frame.inputs = inputs;
    frame.tick = tick;
    if (const auto pending = session.pending.find(tick); pending != session.pending.end()) {
        frame.inputs = pending->second.inputs;
        frame.host_hash = pending->second.hash;
        frame.confirmed = true;
        session.pending.erase(pending);
    }
    step_game(session.game, frame.inputs);
    frame.hash_after = game_hash(session.game);
    session.frames.push_back(std::move(frame));
    check_confirmed(session);
    while (session.frames.size() > static_cast<std::size_t>(session.max_history)) {
        if (!session.frames.front().confirmed) {
            session.recovery_reason = "unconfirmed_history_expired";
            session.recovery_tick = session.frames.front().tick;
            session.needs_snapshot = true;
            return;
        }
        session.frames.pop_front();
    }
}

void confirm_frame(RollbackSession& session, const CanonicalFrame& canonical) {
    if (session.needs_snapshot || canonical.tick <= session.confirmed_through) return;
    const auto found = std::find_if(session.frames.begin(), session.frames.end(),
        [&canonical](const RollbackFrame& frame) { return frame.tick == canonical.tick; });
    if (found == session.frames.end()) {
        if (!session.frames.empty() && canonical.tick < session.frames.front().tick) {
            session.needs_snapshot = true;
            session.recovery_reason = "canonical_history_missing";
            session.recovery_tick = canonical.tick;
        } else {
            session.pending[canonical.tick] = canonical;
        }
        return;
    }
    const std::size_t index = static_cast<std::size_t>(found - session.frames.begin());
    const bool changed = found->inputs != canonical.inputs;
    found->inputs = canonical.inputs;
    found->host_hash = canonical.hash;
    found->confirmed = true;
    if (changed) replay_from(session, index);
    check_confirmed(session);
}

void confirm_host_current(RollbackSession& session) {
    if (session.frames.empty()) return;
    RollbackFrame& frame = session.frames.back();
    frame.host_hash = frame.hash_after;
    frame.confirmed = true;
    check_confirmed(session);
}

std::vector<CanonicalFrame> revise_host_input(RollbackSession& session,
                                             std::uint64_t tick, int owner, Input input) {
    const std::pair<std::uint64_t, Input> entry{tick, input};
    return revise_host_inputs(session, owner, {&entry, 1});
}

std::vector<CanonicalFrame> revise_host_inputs(RollbackSession& session, int owner,
    std::span<const std::pair<std::uint64_t, Input>> inputs) {
    if (owner < 0 || owner >= 4) return {};
    std::size_t index = session.frames.size();
    for (const auto& [tick, input] : inputs) {
        if (tick <= session.input_commit_tick) continue;
        const auto found = std::find_if(session.frames.begin(), session.frames.end(),
            [tick](const RollbackFrame& frame) { return frame.tick == tick; });
        if (found == session.frames.end() || found->inputs[static_cast<std::size_t>(owner)] == input)
            continue;
        found->inputs[static_cast<std::size_t>(owner)] = input;
        index = std::min(index, static_cast<std::size_t>(found - session.frames.begin()));
    }
    if (index == session.frames.size()) return {};
    // Redundant UDP input bundles can repair eight ticks with a single replay.
    replay_from(session, index);
    std::vector<CanonicalFrame> corrected;
    corrected.reserve(session.frames.size() - index);
    for (std::size_t current = index; current < session.frames.size(); ++current) {
        RollbackFrame& frame = session.frames[current];
        frame.host_hash = frame.hash_after;
        frame.confirmed = true;
        corrected.push_back({frame.tick, frame.inputs, frame.hash_after});
    }
    return corrected;
}

void apply_correction_batch(RollbackSession& session,
                            const std::vector<CanonicalFrame>& canonical) {
    if (canonical.empty() || session.needs_snapshot) return;
    // A correction may include history preceding a snapshot, or ticks we have not
    // simulated yet. Verify the retained baseline before trimming the old prefix.
    const auto baseline = session.frames.empty() ? session.game.tick : session.frames.front().tick - 1;
    auto start = canonical.begin();
    while (start != canonical.end() && start->tick <= baseline) ++start;
    if (start != canonical.begin()) {
        const Game& before = session.frames.empty() ? session.game : session.frames.front().before;
        const auto& anchor = *(start - 1);
        if (anchor.tick != baseline || anchor.hash != game_hash(before)) {
            session.needs_snapshot = true;
            session.recovery_reason = "correction_baseline_changed";
            session.recovery_tick = anchor.tick;
            return;
        }
    }
    if (start == canonical.end()) return;
    const std::uint64_t first = start->tick;
    // Pending hashes and confirmations after the revised range belong to the old timeline.
    session.pending.erase(session.pending.lower_bound(first), session.pending.end());
    for (auto& frame : session.frames) if (frame.tick >= first) frame.confirmed = false;
    std::size_t index = session.frames.size();
    for (auto it = start; it != canonical.end(); ++it) {
        if (it->tick > session.game.tick) {
            session.pending[it->tick] = *it;
            continue;
        }
        const auto offset = static_cast<std::size_t>(it->tick - baseline - 1);
        if (offset >= session.frames.size() || session.frames[offset].tick != it->tick) {
            session.needs_snapshot = true;
            session.recovery_reason = "correction_history_missing";
            session.recovery_tick = it->tick;
            return;
        }
        auto& frame = session.frames[offset];
        frame.inputs = it->inputs;
        frame.host_hash = it->hash;
        frame.confirmed = true;
        index = std::min(index, offset);
    }
    session.confirmed_through = std::min(session.confirmed_through, first - 1);
    if (index < session.frames.size()) replay_from(session, index);
    check_confirmed(session);
}

void apply_host_snapshot(RollbackSession& session, const Game& snapshot) {
    session.game = snapshot;
    session.game.sound_count = 0;
    session.game.impact_count = 0;
    session.game.shot_count = 0;
    session.frames.clear();
    session.pending.clear();
    session.confirmed_through = snapshot.tick;
    session.needs_snapshot = false;
    session.recovery_reason = "unspecified";
    session.recovery_tick = 0;
}
