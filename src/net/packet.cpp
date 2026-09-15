#include "../net_codec.hpp"

// PACKETS: Explicit little-endian fields; reads retain a sticky truncation error.
void PacketWriter::u8(std::uint8_t value) { bytes.push_back(value); }
void PacketWriter::u16(std::uint16_t value) {
    for (int bit = 0; bit < 16; bit += 8) u8(static_cast<std::uint8_t>(value >> bit));
}
void PacketWriter::u32(std::uint32_t value) {
    for (int bit = 0; bit < 32; bit += 8) u8(static_cast<std::uint8_t>(value >> bit));
}
void PacketWriter::u64(std::uint64_t value) {
    for (int bit = 0; bit < 64; bit += 8) u8(static_cast<std::uint8_t>(value >> bit));
}
void PacketWriter::i32(int value) { u32(static_cast<std::uint32_t>(value)); }
void PacketWriter::input(const Input& value) {
    i32(value.move.x); i32(value.move.y);
    i32(value.aim.x); i32(value.aim.y);
    u8(static_cast<std::uint8_t>(value.use));
    u8(static_cast<std::uint8_t>(value.cancel_use));
    u8(static_cast<std::uint8_t>(value.pickup));
    u8(static_cast<std::uint8_t>(value.drop));
    u8(static_cast<std::uint8_t>(value.reload));
    u8(static_cast<std::uint8_t>(value.interact));
    u8(static_cast<std::uint8_t>(value.confirm));
    i32(value.select);
}

std::uint8_t PacketReader::u8() {
    if (position >= bytes.size()) { okay = false; return 0; }
    return bytes[position++];
}
std::uint16_t PacketReader::u16() {
    std::uint16_t value = 0;
    for (int bit = 0; bit < 16; bit += 8)
        value = static_cast<std::uint16_t>(value | (static_cast<unsigned int>(u8()) << bit));
    return value;
}
std::uint32_t PacketReader::u32() {
    std::uint32_t value = 0;
    for (int bit = 0; bit < 32; bit += 8)
        value |= static_cast<std::uint32_t>(u8()) << bit;
    return value;
}
std::uint64_t PacketReader::u64() {
    std::uint64_t value = 0;
    for (int bit = 0; bit < 64; bit += 8)
        value |= static_cast<std::uint64_t>(u8()) << bit;
    return value;
}
int PacketReader::i32() {
    return static_cast<int>(static_cast<std::int32_t>(u32()));
}
Input PacketReader::input() {
    Input value;
    value.move = {i32(), i32()};
    value.aim = {i32(), i32()};
    value.use = u8() != 0;
    value.cancel_use = u8() != 0;
    value.pickup = u8() != 0;
    value.drop = u8() != 0;
    value.reload = u8() != 0;
    value.interact = u8() != 0;
    value.confirm = u8() != 0;
    value.select = i32();
    if (value.move.x < -1 || value.move.x > 1 || value.move.y < -1 ||
        value.move.y > 1 || value.aim.x < -512 || value.aim.x > 512 ||
        value.aim.y < -512 || value.aim.y > 512 ||
        value.select < -1 || value.select >= quick_slots) okay = false;
    return value;
}
bool PacketReader::finished() const { return okay && position == bytes.size(); }

// FIELDS: Cells and emitters use the same representation everywhere in a snapshot.
void PacketWriter::cell(Cell value) {
    i32(value.x); i32(value.y);
}
Cell PacketReader::cell() { return {i32(), i32()}; }

void PacketWriter::light(LightEmitter value) {
    i32(value.radius);
    i32(value.strength);
    u8(value.color.red); u8(value.color.green); u8(value.color.blue);
    u8(static_cast<std::uint8_t>(value.shape));
}
LightEmitter PacketReader::light() {
    LightEmitter light;
    light.radius = i32();
    light.strength = i32();
    light.color = {u8(), u8(), u8()};
    light.shape = static_cast<LightShape>(u8());
    if (light.shape > LightShape::Beam) okay = false;
    if (light.radius < 0 || light.radius > 128 ||
        light.strength < 0 || light.strength > 10000) okay = false;
    return light;
}

