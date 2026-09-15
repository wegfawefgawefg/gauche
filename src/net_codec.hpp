#pragma once

#include "game.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

struct PacketWriter {
    std::vector<std::uint8_t> bytes;
    void u8(std::uint8_t value);
    void u16(std::uint16_t value);
    void u32(std::uint32_t value);
    void u64(std::uint64_t value);
    void i32(int value);
    void input(const Input& value);
    void cell(Cell value);
    void light(LightEmitter value);
};

struct PacketReader {
    std::span<const std::uint8_t> bytes;
    std::size_t position = 0;
    bool okay = true;
    std::uint8_t u8();
    std::uint16_t u16();
    std::uint32_t u32();
    std::uint64_t u64();
    int i32();
    Input input();
    Cell cell();
    LightEmitter light();
    bool finished() const;
};

std::vector<std::uint8_t> encode_game(const Game& game);
bool decode_game(std::span<const std::uint8_t> bytes, Game& game, std::string& error);
