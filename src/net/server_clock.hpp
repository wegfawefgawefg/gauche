#pragma once

#include <chrono>
#include <cstdint>

// A room service's HTTP Date plus local monotonic elapsed time. Never changes
// the system clock; local wall-clock corrections cannot invalidate a session.
struct ServerClock {
    std::uint64_t epoch_ms = 0;
    std::uint64_t sampled_ms = 0;
};

inline std::uint64_t steady_milliseconds() {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}

inline std::uint64_t server_time_ms(const ServerClock& clock) {
    if (clock.epoch_ms != 0)
        return clock.epoch_ms + (steady_milliseconds() - clock.sampled_ms);
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
}
