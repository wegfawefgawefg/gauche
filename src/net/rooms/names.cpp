#include "names.hpp"
#include <array>
#include <random>

std::string random_room_player_name() {
    constexpr std::array first{
        "Yuki", "Olga", "Sven", "Ada", "Napoleon", "Cleopatra", "Hiroshi", "Bruno",
        "Frida", "Boris", "Amara", "Saoirse", "Ludwig", "Miyamoto", "Astrid", "Socrates",
        "Zelda", "Dmitri", "Ingrid", "Pablo", "Suleiman", "Beatrice", "Ravi", "Nefertiti"};
    constexpr std::array last{
        "Bonaparte", "Quixote", "Tesla", "Babbage", "Tokugawa", "Beethoven", "da Vinci",
        "Watanabe", "MacDonald", "Gutenberg", "Kowalski", "Tchaikovsky", "Poppins",
        "Hernandez", "Lovelace", "Rasputin", "Nakamura", "Einstein", "Lindstrom",
        "Pickleworth", "Mozart", "Biscotti", "O'Malley", "Fitzsprocket"};
    // IDENTITY: Cosmetic local randomness never consumes deterministic gameplay RNG.
    std::random_device random;
    return std::string(first[random() % first.size()]) + " " + last[random() % last.size()];
}
