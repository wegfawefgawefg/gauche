#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

struct NetEndpoint {
    std::uint32_t address = 0;
    std::uint16_t port = 0;
    friend bool operator==(NetEndpoint, NetEndpoint) = default;
};

struct Datagram {
    NetEndpoint from{};
    std::vector<std::uint8_t> bytes;
};

class UdpSocket {
public:
    UdpSocket() = default;
    UdpSocket(const UdpSocket&) = delete;
    UdpSocket& operator=(const UdpSocket&) = delete;
    ~UdpSocket();
    bool open(std::uint16_t port, std::string& error);
    void close();
    bool send(NetEndpoint to, std::span<const std::uint8_t> bytes, std::string& error);
    bool poll(Datagram& packet, std::string& error);
    std::uint16_t bound_port() const { return bound_port_; }
private:
    std::uintptr_t handle_ = 0;
    std::uint16_t bound_port_ = 0;
};

bool resolve_endpoint(const std::string& host, std::uint16_t port,
                      NetEndpoint& endpoint, std::string& error);
std::string endpoint_text(NetEndpoint endpoint);
