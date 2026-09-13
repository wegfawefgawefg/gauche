#include "../src/net_socket.hpp"

#include <array>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <thread>

int main() {
    UdpSocket sender;
    UdpSocket receiver;
    std::string error;
    if (!sender.open(0, error) || !receiver.open(0, error)) {
        std::fprintf(stderr, "socket open failed: %s\n", error.c_str());
        return 1;
    }
    NetEndpoint destination;
    if (!resolve_endpoint("127.0.0.1", receiver.bound_port(), destination, error)) {
        std::fprintf(stderr, "resolve failed: %s\n", error.c_str());
        return 1;
    }
    constexpr std::array<std::uint8_t, 4> payload{1, 2, 3, 4};
    if (!sender.send(destination, payload, error)) {
        std::fprintf(stderr, "send failed: %s\n", error.c_str());
        return 1;
    }
    Datagram received;
    for (int attempt = 0; attempt < 100; ++attempt) {
        if (receiver.poll(received, error)) {
            if (received.bytes.size() == payload.size() &&
                std::equal(received.bytes.begin(), received.bytes.end(), payload.begin())) {
                std::puts("udp loopback passed");
                return 0;
            }
            std::fputs("udp payload changed\n", stderr);
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    std::fprintf(stderr, "receive timed out: %s\n", error.c_str());
    return 1;
}
