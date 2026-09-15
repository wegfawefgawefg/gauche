#pragma once

#include "../net_codec.hpp"

void write_entity(PacketWriter& writer, const Entity& entity);
Entity read_entity(PacketReader& reader);
