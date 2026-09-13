#include "net_socket.hpp"

#include <array>
#include <cerrno>
#include <cstring>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace {

#ifdef _WIN32
using NativeSocket = SOCKET;
constexpr NativeSocket invalid_socket = INVALID_SOCKET;
bool ready(std::string& error) {
    static const int result = [] { WSADATA data{}; return WSAStartup(MAKEWORD(2, 2), &data); }();
    if (result == 0) return true;
    error = "WSAStartup failed: " + std::to_string(result);
    return false;
}
std::string socket_error() { return "Winsock error " + std::to_string(WSAGetLastError()); }
#else
using NativeSocket = int;
constexpr NativeSocket invalid_socket = -1;
bool ready(std::string&) { return true; }
std::string socket_error() { return std::strerror(errno); }
#endif

NativeSocket native(std::uintptr_t handle) { return static_cast<NativeSocket>(handle - 1); }
std::uintptr_t stored(NativeSocket socket) { return static_cast<std::uintptr_t>(socket) + 1; }

sockaddr_in address_of(NetEndpoint endpoint) {
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = endpoint.address;
    address.sin_port = htons(endpoint.port);
    return address;
}

NetEndpoint endpoint_of(const sockaddr_in& address) {
    return {address.sin_addr.s_addr, ntohs(address.sin_port)};
}

} // namespace

UdpSocket::~UdpSocket() { close(); }

bool UdpSocket::open(std::uint16_t port, std::string& error) {
    close();
    if (!ready(error)) return false;
    const NativeSocket socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socket == invalid_socket) { error = "socket: " + socket_error(); return false; }
    handle_ = stored(socket);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if (::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        error = "bind: " + socket_error(); close(); return false;
    }
#ifdef _WIN32
    u_long nonblocking = 1;
    if (ioctlsocket(socket, FIONBIO, &nonblocking) != 0) {
        error = "ioctlsocket: " + socket_error(); close(); return false;
    }
#else
    if (fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK) != 0) {
        error = "fcntl: " + socket_error(); close(); return false;
    }
#endif
    sockaddr_in bound{};
#ifdef _WIN32
    int length = sizeof(bound);
#else
    socklen_t length = sizeof(bound);
#endif
    if (getsockname(socket, reinterpret_cast<sockaddr*>(&bound), &length) != 0) {
        error = "getsockname: " + socket_error(); close(); return false;
    }
    bound_port_ = ntohs(bound.sin_port);
    return true;
}

void UdpSocket::close() {
    if (handle_ == 0) return;
#ifdef _WIN32
    closesocket(native(handle_));
#else
    ::close(native(handle_));
#endif
    handle_ = 0;
    bound_port_ = 0;
}

bool UdpSocket::send(NetEndpoint to, std::span<const std::uint8_t> bytes,
                     std::string& error) {
    if (handle_ == 0 || bytes.size() > 1200) { error = "Socket closed or packet too large"; return false; }
    const sockaddr_in address = address_of(to);
#ifdef _WIN32
    const int sent = sendto(native(handle_), reinterpret_cast<const char*>(bytes.data()),
                            static_cast<int>(bytes.size()), 0,
                            reinterpret_cast<const sockaddr*>(&address), sizeof(address));
#else
    const ssize_t sent = sendto(native(handle_), bytes.data(), bytes.size(), 0,
                                reinterpret_cast<const sockaddr*>(&address), sizeof(address));
#endif
    if (sent < 0 || static_cast<std::size_t>(sent) != bytes.size()) {
        error = "sendto: " + socket_error(); return false;
    }
    return true;
}

bool UdpSocket::poll(Datagram& packet, std::string& error) {
    if (handle_ == 0) { error = "Socket closed"; return false; }
    std::array<std::uint8_t, 1200> buffer{};
    sockaddr_in from{};
#ifdef _WIN32
    int length = sizeof(from);
    const int received = recvfrom(native(handle_), reinterpret_cast<char*>(buffer.data()),
                                  static_cast<int>(buffer.size()), 0,
                                  reinterpret_cast<sockaddr*>(&from), &length);
    if (received < 0 && WSAGetLastError() == WSAEWOULDBLOCK) return false;
#else
    socklen_t length = sizeof(from);
    const ssize_t received = recvfrom(native(handle_), buffer.data(), buffer.size(), 0,
                                      reinterpret_cast<sockaddr*>(&from), &length);
    if (received < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) return false;
#endif
    if (received < 0) { error = "recvfrom: " + socket_error(); return false; }
    packet.from = endpoint_of(from);
    packet.bytes.assign(buffer.begin(), buffer.begin() + received);
    return true;
}

bool resolve_endpoint(const std::string& host, std::uint16_t port,
                      NetEndpoint& endpoint, std::string& error) {
    if (!ready(error)) return false;
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    addrinfo* result = nullptr;
    const int code = getaddrinfo(host.c_str(), nullptr, &hints, &result);
    if (code != 0 || result == nullptr) {
        error = "Unable to resolve host: " + host;
        return false;
    }
    const auto* address = reinterpret_cast<const sockaddr_in*>(result->ai_addr);
    endpoint = {address->sin_addr.s_addr, port};
    freeaddrinfo(result);
    return true;
}

std::string endpoint_text(NetEndpoint endpoint) {
    const sockaddr_in address = address_of(endpoint);
    std::array<char, INET_ADDRSTRLEN> buffer{};
    if (inet_ntop(AF_INET, &address.sin_addr, buffer.data(),
                  static_cast<socklen_t>(buffer.size())) == nullptr) return "?";
    return std::string{buffer.data()} + ":" + std::to_string(endpoint.port);
}
