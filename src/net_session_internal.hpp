#pragma once

#include "net_session.hpp"

void send_wire(NetSession& session, NetEndpoint to, const PacketWriter& packet);
void host_receive(NetSession& session, const Datagram& datagram,
                  PacketReader& reader, WireKind kind);
void client_receive(NetSession& session, const Datagram& datagram,
                    PacketReader& reader, WireKind kind);
void host_step(NetSession& session, Input local_input);
void client_step(NetSession& session, Input local_input);
void queue_snapshot(NetSession& session, int owner);
void send_snapshot_chunks(NetSession& session, int owner);
void receive_snapshot_chunk(NetSession& session, PacketReader& reader);
void send_history_since(NetSession& session, int owner, std::uint64_t after_tick);
