#pragma once

#include <cstdint>
#include <string>
#include <string_view>

struct NetSession;

struct NetDiagnostics {
    std::string directory;
    std::string log_path;
    std::string last_capture;
    std::uint64_t sent_packets = 0, received_packets = 0;
    std::uint64_t sent_bytes = 0, received_bytes = 0;
    std::uint64_t next_capture_ms = 0, next_report_ms = 0;
    std::uint64_t log_bytes = 0;
    int captures = 0;
    int recovery_count = 0;
    bool recovery_recorded = false;
};

void begin_network_log(NetSession& session);
void network_event(NetSession& session, std::string_view event, int owner = -1,
                   std::uint64_t value = 0);
void capture_network_recovery(NetSession& session);
