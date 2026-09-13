#include "rollback.hpp"

#include <algorithm>

namespace {

void check_confirmed(RollbackSession& session) {
    for (const RollbackFrame& frame : session.frames) {
        if (frame.tick <= session.confirmed_through) continue;
        if (frame.tick != session.confirmed_through + 1 || !frame.confirmed) break;
        if (frame.hash_after != frame.host_hash) {
            session.needs_snapshot = true;
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
    if (owner < 0 || owner >= 4) return {};
    const auto found = std::find_if(session.frames.begin(), session.frames.end(),
        [tick](const RollbackFrame& frame) { return frame.tick == tick; });
    if (found == session.frames.end() || found->inputs[static_cast<std::size_t>(owner)] == input)
        return {};
    const std::size_t index = static_cast<std::size_t>(found - session.frames.begin());
    found->inputs[static_cast<std::size_t>(owner)] = input;
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
    const std::uint64_t first = canonical.front().tick;
    const auto found = std::find_if(session.frames.begin(), session.frames.end(),
        [first](const RollbackFrame& frame) { return frame.tick == first; });
    if (found == session.frames.end()) {
        session.needs_snapshot = true;
        return;
    }
    const std::size_t index = static_cast<std::size_t>(found - session.frames.begin());
    if (index + canonical.size() > session.frames.size()) {
        session.needs_snapshot = true;
        return;
    }
    for (std::size_t offset = 0; offset < canonical.size(); ++offset) {
        RollbackFrame& frame = session.frames[index + offset];
        if (frame.tick != canonical[offset].tick) {
            session.needs_snapshot = true;
            return;
        }
        frame.inputs = canonical[offset].inputs;
        frame.host_hash = canonical[offset].hash;
        frame.confirmed = true;
    }
    for (std::size_t current = index + canonical.size(); current < session.frames.size(); ++current)
        session.frames[current].confirmed = false;
    session.confirmed_through = std::min(session.confirmed_through, first - 1);
    replay_from(session, index);
    check_confirmed(session);
}

void apply_host_snapshot(RollbackSession& session, const Game& snapshot) {
    session.game = snapshot;
    session.game.sound_count = 0;
    session.frames.clear();
    session.pending.clear();
    session.confirmed_through = snapshot.tick;
    session.needs_snapshot = false;
}
