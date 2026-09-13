#include "../src/net_codec.hpp"

#include <cstdio>

int main() {
    Game original;
    start_run(original, 22991);
    for (int tick = 0; tick < 120; ++tick) step_game(original, {});
    if (Entity* player = get_entity(original, original.players[0])) {
        player->burn_ticks = 17;
        player->freeze_ticks = 23;
        player->sleep_ticks = 31;
        player->stun_ticks = 3;
        player->artifacts = 1U << static_cast<unsigned int>(ArtifactKind::Hearth);
    }
    const auto encoded = encode_game(original);
    Game restored;
    std::string error;
    if (!decode_game(encoded, restored, error) || game_hash(restored) != game_hash(original) ||
        encode_game(restored) != encoded) {
        std::fprintf(stderr, "snapshot round-trip failed: %s\n", error.c_str());
        return 1;
    }
    for (std::size_t size = 0; size < encoded.size(); size += 17) {
        Game bad;
        if (decode_game(std::span<const std::uint8_t>{encoded.data(), size}, bad, error)) {
            std::fprintf(stderr, "truncated snapshot accepted at %zu\n", size);
            return 1;
        }
    }
    PacketWriter writer;
    Input input;
    input.move = {-1, 0};
    input.aim = {2, -1};
    input.use = true;
    input.select = 5;
    writer.input(input);
    PacketReader reader{writer.bytes};
    if (reader.input() != input || !reader.finished()) {
        std::fputs("input codec failed\n", stderr);
        return 1;
    }
    std::puts("snapshot codec passed");
    return 0;
}
