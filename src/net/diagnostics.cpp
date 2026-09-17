#include "diagnostics.hpp"
#include "../net_session.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>

void begin_network_log(NetSession& session) {
    auto& log = session.diagnostics;
    if (log.directory.empty()) return;
    const std::string directory = log.directory;
    log = {};
    log.directory = directory;
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error) return;
    const auto stamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    log.log_path = (std::filesystem::path(directory) /
        ("session-" + std::to_string(stamp) + ".log")).string();
    network_event(session, "session_begin", session.local_owner, gameplay_version);
}

void network_event(NetSession& session, std::string_view event, int owner, std::uint64_t value) {
    auto& log = session.diagnostics;
    if (log.log_path.empty() || log.log_bytes >= 2 * 1024 * 1024) return;
    // EVENTS: Callers supply fixed event names and numbers, never credential strings.
    std::ostringstream line;
    line << "ms=" << session.now_ms << " tick=" << session.rollback.game.tick
         << " floor=" << session.rollback.game.run.floor
         << " phase=" << static_cast<int>(session.rollback.game.run.phase)
         << " game_over=" << session.rollback.game.game_over
         << " role=" << static_cast<int>(session.role) << " event=" << event
         << " owner=" << owner << " value=" << value
         << " host_tick=" << session.host_tick << " confirmed=" << session.rollback.confirmed_through
         << " revision=" << session.timeline_revision
         << " rtt_ms=" << session.round_trip_ms << " lead=" << session.prediction_lead_ticks
         << " tx_bytes=" << log.sent_bytes << " rx_bytes=" << log.received_bytes
         << " recoveries=" << log.recovery_count << '\n';
    const std::string text = line.str();
    std::ofstream output(log.log_path, std::ios::app);
    if (output << text) log.log_bytes += text.size();
}

void capture_network_recovery(NetSession& session) {
    auto& log = session.diagnostics;
    if (!session.rollback.needs_snapshot) { log.recovery_recorded = false; return; }
    if (log.recovery_recorded) return;
    log.recovery_recorded = true;
    ++log.recovery_count;
    network_event(session, "recovery_required", session.local_owner,
                  session.rollback.confirmed_through);
    network_event(session, session.rollback.recovery_reason, session.local_owner, session.rollback.recovery_tick);
    if (log.log_path.empty() || log.captures >= 8 || session.now_ms < log.next_capture_ms) return;

    const auto& frames = session.rollback.frames;
    // CAPTURE: Keep the state before the oldest saved input, before a resync replaces it.
    const auto snapshot = encode_game(frames.empty() ? session.rollback.game : frames.front().before);
    PacketWriter capture;
    capture.u32(0x4752504cU); // GRPL: Gauche recovery replay, little-endian packet encoding.
    capture.u16(2);
    capture.u16(wire_version);
    capture.u64(gameplay_version);
    capture.u64(session.rollback.confirmed_through);
    capture.u32(static_cast<std::uint32_t>(snapshot.size()));
    capture.bytes.insert(capture.bytes.end(), snapshot.begin(), snapshot.end());
    capture.u32(static_cast<std::uint32_t>(frames.size()));
    for (const RollbackFrame& frame : frames) {
        capture.u64(frame.tick);
        capture.u32(static_cast<std::uint32_t>(frame.inputs.size()));
        for (const auto& [id,input] : frame.inputs) { capture.i32(id); capture.input(input); }
        capture.u64(frame.hash_after);
        capture.u64(frame.host_hash);
        capture.u8(frame.confirmed ? 1 : 0);
    }
    const std::string path = log.log_path + ".recovery-" + std::to_string(log.captures) + ".grpl";
    std::ofstream output(path, std::ios::binary);
    output.write(reinterpret_cast<const char*>(capture.bytes.data()),
                 static_cast<std::streamsize>(capture.bytes.size()));
    if (!output) { network_event(session, "capture_write_failed"); return; }
    log.last_capture = path;
    ++log.captures;
    log.next_capture_ms = session.now_ms + 5000;
    network_event(session, "recovery_saved", session.local_owner, capture.bytes.size());
}
