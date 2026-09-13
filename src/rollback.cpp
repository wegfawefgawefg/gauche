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

void apply_host_snapshot(RollbackSession& session, const Game& snapshot) {
    session.game = snapshot;
    session.game.sound_count = 0;
    session.frames.clear();
    for (auto it = session.pending.begin(); it != session.pending.end();) {
        if (it->first <= snapshot.tick) it = session.pending.erase(it);
        else ++it;
    }
    session.confirmed_through = snapshot.tick;
    session.needs_snapshot = false;
}
